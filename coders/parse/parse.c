/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 17:51:53 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/23 10:44:34 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../coders.h"

bool	is_valid_num(char **argv)
{
	if (!in_range(str_to_int(argv[1]), 1, 200))
		return (false);
	if (!in_range(str_to_int(argv[7]), 1, 1000))
		return (false);
	return (true);
}

bool	is_valid_time(char **argv)
{
	int	i;

	i = 2;
	while (i <= 6)
	{
		if (!in_range(str_to_int(argv[i]), 1, 1000))
			return (false);
		i++;
	}
	return (true);
}

bool	is_valid_scheduler(char *arg)
{
	if (!strcmp(arg, "fifo") || !strcmp(arg, "edf"))
		return (true);
	return (false);
}

bool	in_range(int value, int min, int max)
{
	return (value >= min && value <= max);
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
		return (free(tab), NULL);
	return (tab);
}
