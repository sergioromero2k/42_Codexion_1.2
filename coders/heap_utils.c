/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: serromer <serromer@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 05:07:49 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/05/25 14:59:53 by serromer         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Swaps two nodes within the heap array of the priority queue.
 *
 * @param pq Pointer to the priority queue structure.
 * @param index Index of the first node to swap.
 * @param padre Index of the second node (typically the parent) to swap.
 */
static void	swap(t_pqueue *pq, int index, int padre)
{
	t_node	temp;

	temp = pq->heap[index];
	pq->heap[index] = pq->heap[padre];
	pq->heap[padre] = temp;
}

static int	has_priority(t_node a, t_node b)
{
	if (a.priority < b.priority)
		return (1);
	if (a.priority == b.priority && a.coder_id > b.coder_id)
		return (1);
	return (0);
}

/**
 * Restores the min-heap property by moving a node up the tree until
 * it is in the correct position based on its priority.
 *
 * @param pq Pointer to the priority queue structure.
 * @param index The starting index of the node to move upward.
 */
void	sift_up(t_pqueue *pq, int index)
{
	int	padre;

	while (index > 0)
	{
		padre = (index - 1) / 2;
		if (has_priority(pq->heap[index], pq->heap[padre]))
		{
			swap(pq, index, padre);
			index = padre;
		}
		else
			index = 0;
	}
}

/**
 * Restores the min-heap property by moving a node down the tree,
 * ensuring that parent nodes always have a higher priority than their children.
 *
 * @param pq Pointer to the priority queue structure.
 * @param index The starting index of the node to move downward.
 */
void	sift_down(t_pqueue *pq, int index)
{
	int	left_child;
	int	right_child;
	int	smallest;

	while (2 * index + 1 < pq->size)
	{
		left_child = 2 * index + 1;
		right_child = 2 * index + 2;
		smallest = left_child;
		if ((right_child < pq->size) && has_priority(pq->heap[right_child],
				pq->heap[left_child]))
			smallest = right_child;
		if (pq->heap[index].priority > pq->heap[smallest].priority
			|| (pq->heap[index].priority == pq->heap[smallest].priority
				&& pq->heap[index].coder_id < pq->heap[smallest].coder_id))
		{
			swap(pq, index, smallest);
			index = smallest;
		}
		else
			index = 0;
	}
}
