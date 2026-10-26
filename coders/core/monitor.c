/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 12:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

static bool	check_burnout(t_table *table, uint64_t now)
{
	int		i;
	t_coder	*coder;

	i = 0;
	while (i < table->nb_cod)
	{
		coder = &table->coders[i];
		if (!coder->completed
			&& now >= coder->last_comp_start + table->ttb)
		{
			coder->state = ST_BURNT;
			table->burned_out = true;
			table->burned_coder_id = coder->id;
			mark_simulation_end(table);
			log_add(table, coder->id, "burned out", CLR_BURN);
			return (true);
		}
		i++;
	}
	return (false);
}

static bool	monitor_tick(t_table *table)
{
	uint64_t	now;
	bool		stop;

	pthread_mutex_lock(&table->pool_mutex);
	stop = table->simulation_end;
	if (!stop && table->start_phase != 0)
	{
		now = now_ms();
		stop = check_burnout(table, now);
		if (!stop)
			grant_dongles(table, now);
	}
	pthread_mutex_unlock(&table->pool_mutex);
	return (stop);
}

void	*monitor_routine(void *arg)
{
	t_table	*table;

	table = arg;
	while (!monitor_tick(table))
		usleep(1000);
	return (NULL);
}
