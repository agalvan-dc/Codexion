/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   init_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/20 20:00:43 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/25 18:18:35 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

static bool	init_coder_cond(t_coder *coder)
{
	if (pthread_cond_init(&coder->cond, NULL))
		return (false);
	return (true);
}

static void	set_dongle_ids(t_coder *coder, int nb, size_t index)
{
	coder->left_dongle = (int)index;
	if (nb == 1)
		coder->right_dongle = 0;
	else
		coder->right_dongle = ((int)index + 1) % nb;
}

static t_coder	*fill_coder(t_coder *coder, size_t index, t_table *table)
{
	int	nb;

	nb = table->nb_cod;
	coder->id = (int)index + 1;
	coder->num_com = 0;
	coder->last_com = 0;
	coder->last_comp_start = 0;
	coder->deadline = 0;
	coder->wait_since = 0;
	coder->request_time = 0;
	coder->state = ST_WAIT;
	set_dongle_ids(coder, nb, index);
	coder->has_pair = false;
	coder->thread = 0;
	coder->next = NULL;
	if (!init_coder_cond(coder))
		return (NULL);
	return (coder);
}

static t_coder	*append_coder(t_coder **aux, t_coder **init, t_coder *coder)
{
	if (!*aux)
		*init = coder;
	else
		(*aux)->next = coder;
	*aux = coder;
	return (*aux);
}

t_coder		*init_coders(t_table *table)
{
	size_t	i;
	t_coder	*coder;
	t_coder	*aux;
	t_coder	*init;

	aux = NULL;
	coder = NULL;
	init = NULL;
	i = 0;
	while (i < (size_t)table->nb_cod)
	{
		coder = malloc(sizeof(t_coder));
		if (!coder)
			return (free_prev(NULL, init));
		memset(coder, 0, sizeof(t_coder));
		if (!fill_coder(coder, i, table))
			return (free_prev(coder, init));
		coder->table = table;
		append_coder(&aux, &init, coder);
		i++;
	}
	if (!coder)
		return (NULL);
	coder->next = init;
	return (coder);
}

