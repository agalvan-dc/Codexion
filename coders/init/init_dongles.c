/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 18:10:32 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

bool	init_dongles(t_table *table)
{
	int	i;

	table->dongles = malloc(sizeof(t_dongle) * (size_t)table->nb_cod);
	if (!table->dongles)
		return (false);
	memset(table->dongles, 0, sizeof(t_dongle) * (size_t)table->nb_cod);
	i = 0;
	while (i < table->nb_cod)
	{
		if (pthread_mutex_init(&table->dongles[i].mutex, NULL))
			return (false);
		table->dongle_ready++;
		i++;
	}
	return (true);
}
