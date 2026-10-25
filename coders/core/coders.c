/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coders.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 02:39:54 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/25 18:02:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

uint64_t	now_ms(void)
{
	struct timeval	tv;

	gettimeofday(&tv, NULL);
	return ((uint64_t)tv.tv_sec * 1000ULL + (uint64_t)tv.tv_usec / 1000ULL);
}

static size_t	append_uint(char *dst, uint64_t value)
{
	char	tmp[20];
	size_t	len;
	size_t	i;

	len = 0;
	if (value == 0)
	{
		dst[0] = '0';
		return (1);
	}
	while (value > 0 && len < sizeof(tmp))
	{
		tmp[len++] = (char)('0' + (value % 10));
		value /= 10;
	}
	i = 0;
	while (len > 0)
		dst[i++] = tmp[--len];
	return (i);
}

static size_t	append_int(char *dst, int value)
{
	size_t	len;

	len = 0;
	if (value < 0)
	{
		dst[len++] = '-';
		value = -value;
	}
	return (len + append_uint(dst + len, (uint64_t)value));
}

void	log_add(t_table *table, int id, const char *msg)
{
	char	line[LOG_LINE_MAX];
	size_t	len;
	size_t	pos;
	size_t	i;

	len = append_uint(line, now_ms() - table->start_ms);
	line[len++] = ' ';
	len += append_int(line + len, id);
	line[len++] = ' ';
	i = 0;
	while (msg[i] && len + 2 < LOG_LINE_MAX)
		line[len++] = msg[i++];
	line[len++] = '\n';
	line[len] = '\0';
	pthread_mutex_lock(&table->log_mutex);
	if (table->log_count < table->log_cap)
	{
		pos = (table->log_head + table->log_count) % table->log_cap;
		i = 0;
		while (i <= len)
		{
			table->logs[pos].text[i] = line[i];
			i++;
		}
		table->log_count++;
		pthread_cond_broadcast(&table->log_cond);
	}
	pthread_mutex_unlock(&table->log_mutex);
}

static void	print_batch(t_log *batch, size_t count)
{
	size_t	i;

	i = 0;
	while (i < count)
	{
		(void)write(1, batch[i].text, strlen(batch[i].text));
		i++;
	}
}

void	*printer_routine(void *arg)
{
	t_table	*table;
	t_log	batch[LOG_BATCH];
	size_t	count;

	table = arg;
	for (;;)
	{
		pthread_mutex_lock(&table->log_mutex);
		while (table->log_count == 0 && !table->printer_stop)
			pthread_cond_wait(&table->log_cond, &table->log_mutex);
		if (table->log_count == 0 && table->printer_stop)
		{
			pthread_mutex_unlock(&table->log_mutex);
			break ;
		}
		count = 0;
		while (count < LOG_BATCH && table->log_count > 0)
		{
			batch[count] = table->logs[table->log_head];
			table->log_head = (table->log_head + 1) % table->log_cap;
			table->log_count--;
			count++;
		}
		pthread_mutex_unlock(&table->log_mutex);
		print_batch(batch, count);
	}
	return (NULL);
}

bool	is_burned_out(t_coder *coder, uint64_t now)
{
	t_table	*table;

	table = coder->table;
	if (!coder || !table)
		return (false);
	if (coder->state == ST_BURNT)
		return (true);
	if (now >= coder->last_comp_start + table->ttb)
		return (true);
	return (false);
}

bool	can_take_pair(t_table *table, int left_idx, int right_idx, uint64_t now)
{
	if (left_idx < 0 || left_idx >= table->nb_cod)
		return (false);
	if (right_idx < 0 || right_idx >= table->nb_cod)
		return (false);
	if (table->dongles[left_idx].in_use)
		return (false);
	if (table->dongles[right_idx].in_use)
		return (false);
	if (table->dongles[left_idx].available_at > now)
		return (false);
	if (table->dongles[right_idx].available_at > now)
		return (false);
	return (true);
}

static void	take_pair(t_coder *coder, uint64_t now)
{
	t_table	*table;
	int		left;
	int		right;

	table = coder->table;
	left = coder->left_dongle;
	right = coder->right_dongle;
	pthread_mutex_lock(&table->dongles[left].mutex);
	pthread_mutex_lock(&table->dongles[right].mutex);
	if (can_take_pair(table, left, right, now))
	{
		table->dongles[left].in_use = true;
		table->dongles[right].in_use = true;
		coder->has_pair = true;
	}
	pthread_mutex_unlock(&table->dongles[right].mutex);
	pthread_mutex_unlock(&table->dongles[left].mutex);
}

void	request_dongles(t_coder *coder)
{
	t_table	*table;
	uint64_t	now;

	table = coder->table;
	now = now_ms();
	pthread_mutex_lock(&table->pool_mutex);
	if (table->simulation_end)
	{
		pthread_mutex_unlock(&table->pool_mutex);
		return ;
	}
	if (is_burned_out(coder, now))
	{
		table->simulation_end = true;
		table->burned_out = true;
		table->burned_coder_id = coder->id;
		coder->state = ST_BURNT;
		pthread_mutex_unlock(&table->pool_mutex);
		log_add(table, coder->id, "burned out");
		return ;
	}
	coder->wait_since = now;
	coder->request_time = now;
	coder->deadline = coder->last_comp_start + table->ttb;
	pthread_mutex_lock(&table->heap_mutex);
	heap_push(table->wait_heap, coder, coder->request_time);
	pthread_mutex_unlock(&table->heap_mutex);
	coder->state = ST_WAIT;
	pthread_mutex_unlock(&table->pool_mutex);
}

void	release_dongles(t_coder *coder)
{
	t_table	*table;
	int		left;
	int		right;
	uint64_t	now;

	table = coder->table;
	left = coder->left_dongle;
	right = coder->right_dongle;
	now = now_ms();
	if (left < 0 || left >= table->nb_cod || right < 0 || right >= table->nb_cod)
		return ;
	pthread_mutex_lock(&table->dongles[left].mutex);
	table->dongles[left].in_use = false;
	table->dongles[left].available_at = now + table->dc;
	pthread_mutex_unlock(&table->dongles[left].mutex);
	pthread_mutex_lock(&table->dongles[right].mutex);
	table->dongles[right].in_use = false;
	table->dongles[right].available_at = now + table->dc;
	pthread_mutex_unlock(&table->dongles[right].mutex);
	coder->has_pair = false;
}

void	end_simulation(t_table *table)
{
	t_coder	*coder;
	int		i;

	pthread_mutex_lock(&table->pool_mutex);
	table->simulation_end = true;
	i = 0;
	coder = NULL;
	if (table->coders)
		coder = table->coders->next;
	while (coder && i < table->nb_cod)
	{
		pthread_cond_broadcast(&coder->cond);
		coder = coder->next;
		i++;
	}
	pthread_mutex_unlock(&table->pool_mutex);
}

static void	prepare_compile(t_coder *coder, t_table *table)
{
	log_add(table, coder->id, "has taken a dongle");
	log_add(table, coder->id, "has taken a dongle");
	pthread_mutex_lock(&table->pool_mutex);
	coder->last_comp_start = now_ms();
	coder->deadline = coder->last_comp_start + table->ttb;
	pthread_mutex_unlock(&table->pool_mutex);
	coder->state = ST_COMPILE;
	log_add(table, coder->id, "is compiling");
	usleep((useconds_t)(table->ttc * 1000));
}

static void	do_debug_refactor(t_coder *coder, t_table *table)
{
	coder->state = ST_DEBUG;
	log_add(table, coder->id, "is debugging");
	usleep((useconds_t)(table->ttd * 1000));
	coder->state = ST_REFACTOR;
	log_add(table, coder->id, "is refactoring");
	usleep((useconds_t)(table->ttr * 1000));
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	t_table	*table;

	coder = arg;
	table = coder->table;
	pthread_mutex_lock(&table->pool_mutex);
	if (table->start_ms == 0)
		table->start_ms = now_ms();
	coder->last_comp_start = table->start_ms;
	coder->deadline = table->start_ms + table->ttb;
	coder->state = ST_WAIT;
	pthread_mutex_unlock(&table->pool_mutex);
	while (!table->simulation_end)
	{
		if (table->simulation_end)
			break ;
		if (coder->num_com >= table->nb_com)
		{
			coder->state = ST_DONE;
			pthread_mutex_lock(&table->pool_mutex);
			table->done_count++;
			if (table->done_count >= table->nb_cod)
				table->simulation_end = true;
			pthread_mutex_unlock(&table->pool_mutex);
			break ;
		}
		if (table->simulation_end)
			break ;
		request_dongles(coder);
		if (table->simulation_end)
			break ;
		pthread_mutex_lock(&table->heap_mutex);
		if (!heap_empty(table->wait_heap) && heap_top(table->wait_heap) == coder)
			heap_pop(table->wait_heap);
		pthread_mutex_unlock(&table->heap_mutex);
		take_pair(coder, now_ms());
		if (!coder->has_pair)
			continue ;
		prepare_compile(coder, table);
		release_dongles(coder);
		do_debug_refactor(coder, table);
		coder->num_com++;
	}
	return (NULL);
}

bool	run_simulation(t_table *table)
{
	bool	started;

	started = start_threads(table);
	join_threads(table);
	return (started);
}
