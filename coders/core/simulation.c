/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   simulation.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 12:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

static void	broadcast_all(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->nb_cod)
	{
		pthread_cond_broadcast(&table->coders[i].cond);
		i++;
	}
}

void	mark_simulation_end(t_table *table)
{
	table->simulation_end = true;
	broadcast_all(table);
}

void	end_simulation(t_table *table)
{
	pthread_mutex_lock(&table->pool_mutex);
	if (!table->simulation_end)
		mark_simulation_end(table);
	pthread_mutex_unlock(&table->pool_mutex);
}

bool	simulation_over(t_table *table)
{
	bool	over;

	pthread_mutex_lock(&table->pool_mutex);
	over = table->simulation_end;
	pthread_mutex_unlock(&table->pool_mutex);
	return (over);
}

bool	run_simulation(t_table *table)
{
	bool	started;

	started = start_threads(table);
	join_threads(table);
	return (started);
}
