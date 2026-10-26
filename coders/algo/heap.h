/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.h                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 14:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
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

t_heap	*heap_create(t_scheduler scheduler, int capacity);
bool	heap_empty(t_heap *heap);
void	heap_destroy(t_heap *heap);
t_coder	*heap_pop(t_heap *heap);
t_coder	*heap_top(t_heap *heap);
void	heap_push(t_heap *heap, t_coder *coder, uint64_t request_time);
void	heap_bubble_up(t_heap *heap, int idx);
void	heap_bubble_down(t_heap *heap, int idx);

#endif
