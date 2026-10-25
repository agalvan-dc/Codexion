/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_dongles.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 18:10:32 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/25 18:11:53 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

t_dongle	*init_dongles(t_table *table)
{
	size_t		i;
	t_dongle	*dongles;
	int			nb_dongle;

	nb_dongle = table->nb_cod;
	if (nb_dongle < 1)
		return (NULL);
	dongles = malloc(sizeof(t_dongle) * (size_t)nb_dongle);
	if (!dongles)
		return (NULL);
	i = 0;
	while (i < (size_t)nb_dongle)
	{
		dongles[i].in_use = false;
		dongles[i].available_at = 0;
		if (pthread_mutex_init(&dongles[i].mutex, NULL))
			return (NULL);
		i++;
	}
	return (dongles);
}
