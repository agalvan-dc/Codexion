/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 14:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/24 14:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef HEAP_H
# define HEAP_H

# include <stdbool.h>
# include <stdlib.h>
# include <stdint.h>

typedef struct s_coder	t_coder;

typedef enum e_scheduler
{
	FIFO,
	EDF,
}	t_scheduler;

typedef struct s_heap_item
{
	t_coder			*coder;
	uint64_t		request_time;
	uint64_t		deadline;
}	t_heap_item;

typedef struct s_heap
{
	t_heap_item		*items;
	int				size;
	int				capacity;
	t_scheduler		scheduler;
}	t_heap;

#endif
