/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 20:00:43 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

static void	fill_coder(t_coder *coder, t_table *table, int index)
{
	coder->id = index + 1;
	coder->num_com = 0;
	coder->last_comp_start = 0;
	coder->deadline = 0;
	coder->request_time = 0;
	coder->state = ST_WAIT;
	coder->left_dongle = index;
	coder->right_dongle = (index + 1) % table->nb_cod;
	coder->has_pair = false;
	coder->table = table;
}

bool	init_coders(t_table *table)
{
	int	i;

	table->coders = malloc(sizeof(t_coder) * (size_t)table->nb_cod);
	if (!table->coders)
		return (false);
	memset(table->coders, 0, sizeof(t_coder) * (size_t)table->nb_cod);
	i = 0;
	while (i < table->nb_cod)
	{
		fill_coder(&table->coders[i], table, i);
		if (pthread_cond_init(&table->coders[i].cond, NULL))
			return (false);
		table->cond_ready++;
		i++;
	}
	return (true);
}
