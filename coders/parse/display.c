/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   display.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/22 14:47:54 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/22 14:48:14 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "coders.h"

void	show_usage(void)
{
	fprintf(stderr, "Usage: ./codexion [nb_cod] [ttb] [ttc] [ttd] [ttr] "
		"[ncr] [dc] [scheduler]\n\n");
	fprintf(stderr, "  nb_cod\tnumber of coders\t(%d-%d)\n",
		1, 200);
	fprintf(stderr, "  ttb\ttime to burnout\t(%d-%d ms)\n",
		1, 1000);
	fprintf(stderr, "  ttc\ttime to compile\t(%d-%d ms)\n",
		1, 1000);
	fprintf(stderr, "  ttd\ttime to debug\t(%d-%d ms)\n",
		1, 1000);
	fprintf(stderr, "  ttr\ttime to refactor\t(%d-%d ms)\n",
		1, 1000);
	fprintf(stderr, "  ncr\tnumber of compiles required\t(%d-%d)\n",
		1, 1000);
	fprintf(stderr, "  dc\tdongle cooldown\t(%d-%d ms)\n",
		1, 1000);
	fprintf(stderr, "  scheduler\t\"fifo\" or \"edf\"\n");
}
