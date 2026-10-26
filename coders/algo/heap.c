/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/10/24 14:00:00 by agalvan-          #+#    #+#             */
/*   Updated: 2026/10/26 12:00:00 by agalvan-         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../coders.h"

t_heap	*heap_create(t_scheduler scheduler, int capacity)
{
	t_heap	*heap;

	heap = malloc(sizeof(t_heap));
	if (!heap)
		return (NULL);
	heap->items = malloc(sizeof(t_heap_item) * (size_t)capacity);
	if (!heap->items)
	{
		free(heap);
		return (NULL);
	}
	memset(heap->items, 0, sizeof(t_heap_item) * (size_t)capacity);
	heap->size = 0;
	heap->capacity = capacity;
	heap->scheduler = scheduler;
	return (heap);
}

t_coder	*heap_top(t_heap *heap)
{
	if (!heap || heap->size == 0)
		return (NULL);
	return (heap->items[0].coder);
}

t_coder	*heap_pop(t_heap *heap)
{
	t_coder	*coder;

	if (!heap || heap->size == 0)
		return (NULL);
	coder = heap->items[0].coder;
	heap->size--;
	if (heap->size > 0)
	{
		heap->items[0] = heap->items[heap->size];
		heap_bubble_down(heap, 0);
	}
	return (coder);
}

bool	heap_empty(t_heap *heap)
{
	if (!heap)
		return (true);
	return (heap->size == 0);
}

void	heap_destroy(t_heap *heap)
{
	if (!heap)
		return ;
	free(heap->items);
	free(heap);
}
