/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   free.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/23 10:03:59 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/23 11:20:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

static void	free_coders(t_coder *head, int count)
{
	t_coder	*next;
	int		i;

	i = 0;
	while (head && i < count)
	{
		next = head->next;
		pthread_coder_destroy(head);
		free(head);
		head = next;
		i++;
	}
}

void	*free_prev(void *failed, void *head)
{
	t_coder	*node;
	t_coder	*next;

	if (failed)
		free(failed);
	node = head;
	while (node)
	{
		next = node->next;
		pthread_coder_destroy(node);
		free(node);
		node = next;
	}
	return (NULL);
}

bool	pthread_coder_destroy(t_coder *coder)
{
	if (!coder)
		return (false);
	pthread_cond_destroy(&coder->cond);
	return (false);
}

static void	free_dongles(t_dongle *dongles, int count)
{
	int	i;

	i = 0;
	if (!dongles)
		return ;
	while (i < count)
	{
		pthread_mutex_destroy(&dongles[i].mutex);
		i++;
	}
}

void	pthread_destroy(t_table *table)
{
	if (!table)
		return ;
	pthread_mutex_destroy(&table->log_mutex);
	pthread_mutex_destroy(&table->pool_mutex);
	pthread_mutex_destroy(&table->heap_mutex);
	pthread_cond_destroy(&table->log_cond);
	free_coders(table->coders, table->nb_cod);
	free_dongles(table->dongles, table->nb_cod);
	free(table->dongles);
	if (table->wait_heap)
		heap_destroy(table->wait_heap);
	free(table->logs);
	free(table->threads);
	table->coders = NULL;
	table->dongles = NULL;
	table->wait_heap = NULL;
	table->logs = NULL;
	table->threads = NULL;
}
