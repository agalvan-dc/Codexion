/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_ops.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 17:35:58 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

static void	heap_swap(t_heap_item *a, t_heap_item *b)
{
	t_heap_item	tmp;

	tmp = *a;
	*a = *b;
	*b = tmp;
}

static bool	heap_less(t_heap *heap, t_heap_item *a, t_heap_item *b)
{
	if (heap->scheduler == FIFO)
	{
		if (a->request_time != b->request_time)
			return (a->request_time < b->request_time);
		return (a->coder->id < b->coder->id);
	}
	if (a->deadline != b->deadline)
		return (a->deadline < b->deadline);
	if (a->request_time != b->request_time)
		return (a->request_time < b->request_time);
	return (a->coder->id < b->coder->id);
}

void	heap_bubble_up(t_heap *heap, int idx)
{
	int	parent;

	while (idx > 0)
	{
		parent = (idx - 1) / 2;
		if (!heap_less(heap, &heap->items[idx], &heap->items[parent]))
			break ;
		heap_swap(&heap->items[idx], &heap->items[parent]);
		idx = parent;
	}
}

void	heap_bubble_down(t_heap *heap, int idx)
{
	int	child;

	while (2 * idx + 1 < heap->size)
	{
		child = 2 * idx + 1;
		if (child + 1 < heap->size
			&& heap_less(heap, &heap->items[child + 1], &heap->items[child]))
			child++;
		if (!heap_less(heap, &heap->items[child], &heap->items[idx]))
			break ;
		heap_swap(&heap->items[idx], &heap->items[child]);
		idx = child;
	}
}

void	heap_push(t_heap *heap, t_coder *coder, uint64_t request_time)
{
	if (!heap || !coder || heap->size >= heap->capacity)
		return ;
	heap->items[heap->size].coder = coder;
	heap->items[heap->size].request_time = request_time;
	heap->items[heap->size].deadline = coder->deadline;
	heap_bubble_up(heap, heap->size);
	heap->size++;
}
