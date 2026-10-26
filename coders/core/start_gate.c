/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_gate.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 12:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

bool	wait_start_gate(t_coder *coder)
{
	t_table	*table;
	bool	ok;

	table = coder->table;
	pthread_mutex_lock(&table->pool_mutex);
	table->threads_ready++;
	pthread_cond_broadcast(&table->start_cond);
	while (!table->simulation_end && table->start_phase == 0)
		pthread_cond_wait(&table->start_cond, &table->pool_mutex);
	while (!table->simulation_end && coder->id % 2 != 0
		&& table->start_phase < 2)
		pthread_cond_wait(&table->start_cond, &table->pool_mutex);
	ok = !table->simulation_end;
	pthread_mutex_unlock(&table->pool_mutex);
	return (ok);
}

static void	set_start_times(t_table *table)
{
	int	i;

	table->start_ms = now_ms();
	table->start_even_total = 0;
	table->start_even_done = 0;
	i = 0;
	while (i < table->nb_cod)
	{
		table->coders[i].last_comp_start = table->start_ms;
		table->coders[i].deadline = table->start_ms + table->ttb;
		if ((i + 1) % 2 == 0)
			table->start_even_total++;
		i++;
	}
}

bool	open_start_gate(t_table *table)
{
	pthread_mutex_lock(&table->pool_mutex);
	while (!table->simulation_end && table->threads_ready < table->nb_cod)
		pthread_cond_wait(&table->start_cond, &table->pool_mutex);
	if (table->simulation_end)
	{
		pthread_mutex_unlock(&table->pool_mutex);
		return (false);
	}
	set_start_times(table);
	table->start_phase = 1;
	if (table->start_even_total == 0)
		table->start_phase = 2;
	pthread_cond_broadcast(&table->start_cond);
	pthread_mutex_unlock(&table->pool_mutex);
	return (true);
}

void	mark_even_started(t_table *table, t_coder *coder)
{
	if (table->start_phase != 1 || coder->id % 2 != 0)
		return ;
	table->start_even_done++;
	if (table->start_even_done >= table->start_even_total)
	{
		table->start_phase = 2;
		pthread_cond_broadcast(&table->start_cond);
	}
}
