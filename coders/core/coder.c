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

static bool	after_compile(t_coder *coder)
{
	t_table	*table;
	bool	done;

	table = coder->table;
	done = false;
	pthread_mutex_lock(&table->pool_mutex);
	if (table->simulation_end)
	{
		pthread_mutex_unlock(&table->pool_mutex);
		return (false);
	}
	coder->num_com++;
	if (coder->num_com >= table->nb_com)
	{
		coder->state = ST_DONE;
		table->done_count++;
		done = true;
		if (table->done_count >= table->nb_cod)
			mark_simulation_end(table);
	}
	pthread_mutex_unlock(&table->pool_mutex);
	return (!done);
}

static bool	wait_and_compile(t_coder *coder)
{
	t_table	*table;

	table = coder->table;
	if (!request_dongles(coder))
		return (false);
	if (!enter_state(coder, ST_COMPILE, "is compiling"))
	{
		release_dongles(coder);
		return (false);
	}
	sleep_ms(table, table->ttc);
	release_dongles(coder);
	return (after_compile(coder));
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

static bool	coder_cycle(t_coder *coder)
{
	if (simulation_over(coder->table))
		return (false);
	if (!wait_and_compile(coder))
		return (false);
	return (do_debug_refactor(coder));
}

void	*coder_routine(void *arg)
{
	t_coder	*coder;

	coder = arg;
	while (coder_cycle(coder))
		;
	return (NULL);
}
