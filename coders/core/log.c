/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   log.c                                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/26 12:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

static size_t	put_uint(char *dst, uint64_t value)
{
	char	tmp[20];
	size_t	len;
	size_t	i;

	len = 0;
	if (value == 0)
	{
		dst[0] = '0';
		return (1);
	}
	while (value > 0 && len < sizeof(tmp))
	{
		tmp[len++] = (char)('0' + (value % 10));
		value /= 10;
	}
	i = 0;
	while (len > 0)
		dst[i++] = tmp[--len];
	return (i);
}

static size_t	put_int(char *dst, int value)
{
	size_t	len;

	len = 0;
	if (value < 0)
	{
		dst[len++] = '-';
		value = -value;
	}
	return (len + put_uint(dst + len, (uint64_t)value));
}

static size_t	put_str(char *dst, const char *src)
{
	size_t	len;

	len = 0;
	while (src[len])
	{
		dst[len] = src[len];
		len++;
	}
	return (len);
}

static void	write_all(const char *buf, size_t len)
{
	ssize_t	ret;

	while (len > 0)
	{
		ret = write(1, buf, len);
		if (ret <= 0)
			return ;
		buf += ret;
		len -= (size_t)ret;
	}
}

void	log_add(t_table *table, int id, const char *msg)
{
	char	line[LOG_LINE_MAX];
	size_t	len;

	len = put_uint(line, now_ms() - table->start_ms);
	line[len++] = ' ';
	len += put_int(line + len, id);
	line[len++] = ' ';
	len += put_str(line + len, msg);
	line[len++] = '\n';
	pthread_mutex_lock(&table->log_mutex);
	write_all(line, len);
	pthread_mutex_unlock(&table->log_mutex);
}
