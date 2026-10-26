/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   dongles.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 12:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

static bool	pair_ready(t_table *table, t_coder *coder, uint64_t now)
{
	t_dongle	*left;
	t_dongle	*right;

	if (coder->left_dongle == coder->right_dongle)
		return (false);
	left = &table->dongles[coder->left_dongle];
	right = &table->dongles[coder->right_dongle];
	return (!left->in_use && left->available_at <= now
		&& !right->in_use && right->available_at <= now);
}

void	grant_dongles(t_table *table, uint64_t now)
{
	t_coder	*coder;

	while (!table->simulation_end && !heap_empty(table->wait_heap))
	{
		coder = heap_top(table->wait_heap);
		lock_pair(table, coder);
		if (!pair_ready(table, coder, now))
		{
			unlock_pair(table, coder);
			return ;
		}
		table->dongles[coder->left_dongle].in_use = true;
		table->dongles[coder->right_dongle].in_use = true;
		unlock_pair(table, coder);
		heap_pop(table->wait_heap);
		coder->has_pair = true;
		coder->last_comp_start = now;
		coder->deadline = now + table->ttb;
		coder->state = ST_COMPILE;
		log_add(table, coder->id, "has taken a dongle");
		log_add(table, coder->id, "has taken a dongle");
		pthread_cond_broadcast(&coder->cond);
	}
}

static bool	wait_for_pair(t_table *table, t_coder *coder)
{
	bool	ok;

	while (!coder->has_pair && !table->simulation_end)
		pthread_cond_wait(&coder->cond, &table->pool_mutex);
	ok = coder->has_pair;
	if (ok && table->simulation_end)
	{
		lock_pair(table, coder);
		table->dongles[coder->left_dongle].in_use = false;
		table->dongles[coder->right_dongle].in_use = false;
		unlock_pair(table, coder);
		coder->has_pair = false;
		ok = false;
	}
	return (ok);
}

bool	request_dongles(t_coder *coder)
{
	t_table		*table;
	uint64_t	now;
	bool		ok;

	table = coder->table;
	pthread_mutex_lock(&table->pool_mutex);
	now = now_ms();
	if (table->simulation_end)
	{
		pthread_mutex_unlock(&table->pool_mutex);
		return (false);
	}
	coder->request_time = now;
	coder->deadline = coder->last_comp_start + table->ttb;
	coder->state = ST_WAIT;
	heap_push(table->wait_heap, coder, now);
	grant_dongles(table, now);
	ok = wait_for_pair(table, coder);
	pthread_mutex_unlock(&table->pool_mutex);
	return (ok);
}

void	release_dongles(t_coder *coder)
{
	t_table		*table;
	uint64_t	now;

	table = coder->table;
	pthread_mutex_lock(&table->pool_mutex);
	if (!coder->has_pair)
	{
		pthread_mutex_unlock(&table->pool_mutex);
		return ;
	}
	now = now_ms();
	lock_pair(table, coder);
	table->dongles[coder->left_dongle].in_use = false;
	table->dongles[coder->left_dongle].available_at = now + table->dc;
	table->dongles[coder->right_dongle].in_use = false;
	table->dongles[coder->right_dongle].available_at = now + table->dc;
	unlock_pair(table, coder);
	coder->has_pair = false;
	grant_dongles(table, now);
	pthread_mutex_unlock(&table->pool_mutex);
}
