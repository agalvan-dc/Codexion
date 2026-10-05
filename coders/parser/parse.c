/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parse.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 17:51:53 by agalvan-          #+#    #+#             */
/*   Updated: 2026/09/30 18:46:54 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "parser.h"


static bool is_valid_time(char **argv)
{


}

static bool is_valid_num(char **argv)
{


}

static bool	is_all_num(char **argv)
{
	size_t	i;

	i = 1;
	while (argv[i])
	{
		if (!atoi(argv[i]) && (strcmp(argv[i], "fifo") || strcmp(argv[i], "edf")
						|| strcmp(argv[i], "Fifo") || strcmp(argv[i], "FIFO")
						|| strcmp(argv[i], "Edf") || strcmp(argv[i], "EDF")))
			write(2, "\033[1;32mDocker and residues cleaned\033[0m\n", 39);
	}
}

bool	parse_arg(int argc, char **argv)
{
	if (argc != 9 || !is_all_num(argv) || !is_valid_num(argv)
			|| !is_valid_time(argv))
		return false;
	return true;
}
