/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log_color.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 12:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

size_t	put_coder_color(char *dst, int id)
{
	uint64_t	code;
	size_t		len;

	code = 100 + (((uint64_t)id * 37) % 132);
	len = 0;
	dst[len++] = '\033';
	dst[len++] = '[';
	dst[len++] = '3';
	dst[len++] = '8';
	dst[len++] = ';';
	dst[len++] = '5';
	dst[len++] = ';';
	dst[len++] = (char)('0' + code / 100);
	dst[len++] = (char)('0' + (code / 10) % 10);
	dst[len++] = (char)('0' + code % 10);
	dst[len++] = 'm';
	return (len);
}
