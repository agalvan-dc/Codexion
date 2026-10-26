/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_sync.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 12:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

bool	init_sync(t_table *table)
{
	if (pthread_mutex_init(&table->pool_mutex, NULL))
		return (false);
	table->pool_ready = true;
	if (pthread_mutex_init(&table->log_mutex, NULL))
		return (false);
	table->log_ready = true;
	if (pthread_cond_init(&table->start_cond, NULL))
		return (false);
	table->start_ready = true;
	return (true);
}
