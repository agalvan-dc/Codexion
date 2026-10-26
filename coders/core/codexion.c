/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 17:34:35 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

int	main(int argc, char **argv)
{
	t_table	*table;
	bool	started;

	table = parse_arg(argc, argv);
	if (!table)
		return (1);
	started = run_simulation(table);
	pthread_destroy(table);
	free(table);
	if (!started)
		return (1);
	return (0);
}
