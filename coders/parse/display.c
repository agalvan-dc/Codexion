/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 14:47:54 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

void	show_usage(void)
{
	fprintf(stderr, "Usage: ./codexion number_of_coders time_to_burnout "
		"time_to_compile time_to_debug time_to_refactor "
		"number_of_compiles_required dongle_cooldown scheduler\n\n");
	fprintf(stderr, "  number_of_coders\t\tpositive integer\n");
	fprintf(stderr, "  time_to_burnout\t\tnon-negative integer (ms)\n");
	fprintf(stderr, "  time_to_compile\t\tnon-negative integer (ms)\n");
	fprintf(stderr, "  time_to_debug\t\tnon-negative integer (ms)\n");
	fprintf(stderr, "  time_to_refactor\t\tnon-negative integer (ms)\n");
	fprintf(stderr, "  number_of_compiles_required\tpositive integer\n");
	fprintf(stderr, "  dongle_cooldown\t\tnon-negative integer (ms)\n");
	fprintf(stderr, "  scheduler\t\t\t\"fifo\" or \"edf\"\n");
}
