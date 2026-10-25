/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 14:10:12 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/23 10:50:06 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

int	str_to_int(char *str)
{
	int	num;

	num = 0;
	if (!*str)
		return (-1);
	while (*str)
	{
		if (*str < '0' || *str > '9')
			return (-1);
		if (num > (INT_MAX - (*str - '0')) / 10)
			return (-1);
		num = num * 10 + (*str - '0');
		str++;
	}
	return (num);
}


static bool	init_table_sync(t_table *tab)
{
	if (pthread_mutex_init(&tab->pool_mutex, NULL))
		return (false);
	if (pthread_mutex_init(&tab->log_mutex, NULL))
		return (false);
	if (pthread_mutex_init(&tab->heap_mutex, NULL))
		return (false);
	if (pthread_cond_init(&tab->log_cond, NULL))
		return (false);
	return (true);
}

static void	table_fields(t_table *tab, char **argv)
{
	tab->nb_cod = str_to_int(argv[1]);
	tab->ttb = (uint64_t)str_to_int(argv[2]);
	tab->ttc = (uint64_t)str_to_int(argv[3]);
	tab->ttd = (uint64_t)str_to_int(argv[4]);
	tab->ttr = (uint64_t)str_to_int(argv[5]);
	tab->nb_com = str_to_int(argv[6]);
	tab->dc = (uint64_t)str_to_int(argv[7]);
	tab->simulation_end = false;
	tab->burned_out = false;
	tab->done_count = 0;
	tab->burned_coder_id = 0;
	if (!strcmp(argv[8], "fifo"))
		tab->scheduler = FIFO;
	else
		tab->scheduler = EDF;
}

bool	fill_table(t_table *tab, char **argv)
{
	table_fields(tab, argv);
	if (!init_table_sync(tab))
	{
		pthread_destroy(tab);
		return (false);
	}
	tab->wait_heap = heap_create(tab->scheduler, tab->nb_cod + 1);
	if (!tab->wait_heap)
	{
		pthread_destroy(tab);
		return (false);
	}
	tab->coders = init_coders(tab);
	tab->dongles = init_dongles(tab);
	if (!tab->coders || !tab->dongles
		|| !init_logs(tab) || !init_waiters(tab) || !init_threads(tab))
	{
		pthread_destroy(tab);
		return (false);
	}
	return (true);
}
