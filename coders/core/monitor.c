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
		if (coder->state != ST_DONE
			&& now >= coder->last_comp_start + table->ttb)
		{
			coder->state = ST_BURNT;
			table->burned_out = true;
			table->burned_coder_id = coder->id;
			mark_simulation_end(table);
			log_add(table, coder->id, "burned out");
			return (true);
		}
		i++;
	}
	return (false);
}

void	*monitor_routine(void *arg)
{
	t_table		*table;
	uint64_t	now;

	table = arg;
	while (1)
	{
		pthread_mutex_lock(&table->pool_mutex);
		if (table->simulation_end)
		{
			pthread_mutex_unlock(&table->pool_mutex);
			break ;
		}
		now = now_ms();
		if (check_burnout(table, now))
		{
			pthread_mutex_unlock(&table->pool_mutex);
			break ;
		}
		grant_dongles(table, now);
		pthread_mutex_unlock(&table->pool_mutex);
		usleep(1000);
	}
	return (NULL);
}
