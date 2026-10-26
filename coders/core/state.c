/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   state.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 12:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

bool	enter_state(t_coder *coder, t_state state, const char *msg)
{
	t_table		*table;
	const char	*color;

	table = coder->table;
	pthread_mutex_lock(&table->pool_mutex);
	if (table->simulation_end)
	{
		pthread_mutex_unlock(&table->pool_mutex);
		return (false);
	}
	color = CLR_COMPILE;
	if (state == ST_DEBUG)
		color = CLR_DEBUG;
	if (state == ST_REFACTOR)
		color = CLR_REFACTOR;
	coder->state = state;
	log_add(table, coder->id, msg, color);
	pthread_mutex_unlock(&table->pool_mutex);
	return (true);
}
