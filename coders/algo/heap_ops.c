/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_ops.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/25 17:35:58 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/25 17:40:21 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "../coders.h"



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
		heap_swap_items(&heap->items[idx], &heap->items[parent]);
		idx = parent;
	}
}

 void	heap_bubble_down(t_heap *heap, int idx)
{
	int	left;
	int	right;
	int	smallest;

	while (1)
	{
		smallest = idx;
		left = 2 * idx + 1;
		right = 2 * idx + 2;
		if (left < heap->size && heap_less(heap, &heap->items[left], &heap->items[smallest]))
			smallest = left;
		if (right < heap->size && heap_less(heap, &heap->items[right], &heap->items[smallest]))
			smallest = right;
		if (smallest == idx)
			break ;
		heap_swap_items(&heap->items[idx], &heap->items[smallest]);
		idx = smallest;
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


