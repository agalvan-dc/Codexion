/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_threads.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 12:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

bool	init_threads(t_table *table)
{
	table->threads = malloc(sizeof(pthread_t) * (size_t)table->nb_cod);
	if (!table->threads)
		return (false);
	table->thread_count = 0;
	table->monitor_started = false;
	table->threads_ready = 0;
	table->start_phase = 0;
	table->start_even_total = 0;
	table->start_even_done = 0;
	return (true);
}

static bool	spawn_coders(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->nb_cod)
	{
		if (pthread_create(&table->threads[i], NULL,
				coder_routine, &table->coders[i]))
		{
			end_simulation(table);
			return (false);
		}
		table->thread_count++;
		i++;
	}
	return (true);
}

bool	start_threads(t_table *table)
{
	if (pthread_create(&table->monitor, NULL, monitor_routine, table))
		return (false);
	table->monitor_started = true;
	if (!spawn_coders(table))
		return (false);
	return (open_start_gate(table));
}

void	join_threads(t_table *table)
{
	int	i;

	i = 0;
	while (i < table->thread_count)
	{
		pthread_join(table->threads[i], NULL);
		i++;
	}
	table->thread_count = 0;
	if (!table->monitor_started)
		return ;
	end_simulation(table);
	pthread_join(table->monitor, NULL);
	table->monitor_started = false;
}
