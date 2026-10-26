/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   coder.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/10 02:39:54 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

static bool	mark_completed(t_table *table, t_coder *coder, bool *completed)
{
	coder->state = ST_DONE;
	coder->completed = true;
	*completed = true;
	table->done_count++;
	if (table->done_count < table->nb_cod)
		return (true);
	mark_simulation_end(table);
	return (false);
}

static bool	after_compile(t_coder *coder, bool *completed)
{
	t_table	*table;
	bool	keep_going;

	table = coder->table;
	*completed = false;
	pthread_mutex_lock(&table->pool_mutex);
	if (table->simulation_end)
	{
		pthread_mutex_unlock(&table->pool_mutex);
		return (false);
	}
	coder->num_com++;
	keep_going = true;
	if (coder->num_com >= table->nb_com)
		keep_going = mark_completed(table, coder, completed);
	pthread_mutex_unlock(&table->pool_mutex);
	return (keep_going);
}

static bool	wait_and_compile(t_coder *coder, bool *completed)
{
	t_table	*table;

	table = coder->table;
	*completed = false;
	if (!request_dongles(coder))
		return (false);
	if (!enter_state(coder, ST_COMPILE, "is compiling"))
	{
		release_dongles(coder);
		return (false);
	}
	sleep_ms(table, table->ttc);
	release_dongles(coder);
	return (after_compile(coder, completed));
}

static bool	do_debug_refactor(t_coder *coder)
{
	t_table	*table;

	table = coder->table;
	if (!enter_state(coder, ST_DEBUG, "is debugging"))
		return (false);
	if (!sleep_ms(table, table->ttd))
		return (false);
	if (!enter_state(coder, ST_REFACTOR, "is refactoring"))
		return (false);
	return (sleep_ms(table, table->ttr));
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;
	bool	completed;

	coder = arg;
	if (!wait_start_gate(coder))
		return (NULL);
	while (!simulation_over(coder->table))
	{
		if (!wait_and_compile(coder, &completed))
			break ;
		if (!do_debug_refactor(coder))
			break ;
		if (completed)
			break ;
	}
	return (NULL);
}
