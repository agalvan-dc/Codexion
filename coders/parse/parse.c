/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 17:51:53 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

static bool	is_valid_num(char **argv)
{
	int	n;

	n = str_to_int(argv[1]);
	if (n < 1 || n == INT_MAX)
		return (false);
	n = str_to_int(argv[6]);
	return (n >= 1);
}

static bool	is_valid_time(char **argv)
{
	if (str_to_int(argv[2]) < 0)
		return (false);
	if (str_to_int(argv[3]) < 0)
		return (false);
	if (str_to_int(argv[4]) < 0)
		return (false);
	if (str_to_int(argv[5]) < 0)
		return (false);
	if (str_to_int(argv[7]) < 0)
		return (false);
	return (true);
}

static bool	is_valid_scheduler(char *arg)
{
	if (!strcmp(arg, "fifo") || !strcmp(arg, "edf"))
		return (true);
	return (false);
}

t_table	*parse_arg(int argc, char **argv)
{
	t_table	*tab;

	if (argc != 9)
	{
		show_usage();
		return (NULL);
	}
	if (!is_valid_num(argv) || !is_valid_time(argv)
		|| !is_valid_scheduler(argv[8]))
	{
		show_usage();
		return (NULL);
	}
	tab = malloc(sizeof(t_table));
	if (!tab)
		return (NULL);
	memset(tab, 0, sizeof(t_table));
	if (!fill_table(tab, argv))
	{
		free(tab);
		return (NULL);
	}
	return (tab);
}
