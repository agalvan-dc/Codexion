/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 10:03:59 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

static void	destroy_coders(t_table *table)
{
	int	i;

	i = 0;
	while (table->coders && i < table->cond_ready)
	{
		pthread_cond_destroy(&table->coders[i].cond);
		i++;
	}
}

static void	destroy_dongles(t_table *table)
{
	int	i;

	i = 0;
	while (table->dongles && i < table->dongle_ready)
	{
		pthread_mutex_destroy(&table->dongles[i].mutex);
		i++;
	}
}

static void	destroy_sync(t_table *table)
{
	if (table->log_ready)
		pthread_mutex_destroy(&table->log_mutex);
	if (table->pool_ready)
		pthread_mutex_destroy(&table->pool_mutex);
}

static void	free_arrays(t_table *table)
{
	if (table->wait_heap)
		heap_destroy(table->wait_heap);
	free(table->coders);
	free(table->dongles);
	free(table->threads);
	table->wait_heap = NULL;
	table->coders = NULL;
	table->dongles = NULL;
	table->threads = NULL;
}

void	pthread_destroy(t_table *table)
{
	if (!table)
		return ;
	destroy_coders(table);
	destroy_dongles(table);
	destroy_sync(table);
	free_arrays(table);
}
