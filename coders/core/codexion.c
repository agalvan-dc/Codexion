/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/09/28 17:34:35 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/25 18:01:19 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

int	main(int argc, char **argv)
{
	t_table	*table;

	table = parse_arg(argc, argv);
	if (!table)
		return (1);
	if (!run_simulation(table))
	{
		pthread_destroy(table);
		free(table);
		return (1);
	}
	pthread_destroy(table);
	free(table);
	return (0);
}
