/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_threads.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 12:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/25 17:42:50 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

bool	init_logs(t_table *table)
{
	size_t	capacity;

	capacity = (size_t)5 * (size_t)table->nb_cod * (size_t)table->nb_com;
	capacity += (size_t)5 * (size_t)table->nb_cod;
	if (capacity == 0)
		capacity = 1;
	table->logs = malloc(capacity * sizeof(t_log));
	if (!table->logs)
		return (false);
	table->log_cap = capacity;
	table->log_count = 0;
	table->log_head = 0;
	return (true);
}

bool	init_waiters(t_table *table)
{
	(void)table;
	return (true);
}

bool	init_threads(t_table *table)
{
	table->threads = malloc(sizeof(pthread_t) * (size_t)table->nb_cod);
	if (!table->threads)
		return (false);
	table->thread_count = 0;
	table->printer_started = false;
	table->printer_stop = false;
	return (true);
}

bool	start_threads(t_table *table)
{
	t_coder	*coder;
	int		i;

	if (!table->coders)
		return (false);
	table->start_ms = now_ms();
	table->printer_stop = false;
	if (pthread_create(&table->printer, NULL, printer_routine, table))
		return (false);
	table->printer_started = true;
	coder = table->coders->next;
	i = 0;
	while (i < table->nb_cod)
	{
		if (pthread_create(&table->threads[i], NULL, coder_routine, coder))
		{
			end_simulation(table);
			return (false);
		}
		table->thread_count++;
		coder = coder->next;
		i++;
	}
	return (true);
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
	if (!table->printer_started)
		return ;
	pthread_mutex_lock(&table->log_mutex);
	table->printer_stop = true;
	pthread_cond_broadcast(&table->log_cond);
	pthread_mutex_unlock(&table->log_mutex);
	pthread_join(table->printer, NULL);
	table->printer_started = false;
}
