/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heap.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 03:40:19 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/05/26 08:50:25 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Allocates memory for the priority queue's heap
 * and sets up its initial capacity.
 * 
 * @param pq Pointer to the priority queue structure.
 * @param capacity Maximum number of nodes the queue can hold.
 * @return 0 on success, or 1 if memory allocation fails.
 */
int	pqueue_init(t_pqueue *pq, int capacity)
{
	pq->heap = malloc(capacity * sizeof(t_node));
	if (!(pq->heap))
		return (1);
	pq->size = 0;
	pq->capacity = capacity;
	return (0);
}

/**
 * Inserts a new coder into the priority queue and reorders
 * the heap to maintain priority logic.
 * 
 * @param pq Pointer to the priority queue structure.
 * @param coder_id Identifier of the coder to be added.
 * @param priority The priority value used to determine the coder's position.
 */
void	pqueue_push(t_pqueue *pq, int coder_id, long long priority)
{
	if (pq->size >= pq->capacity)
		return ;
	pq->heap[pq->size].coder_id = coder_id;
	pq->heap[pq->size].priority = priority;
	pq->size++;
	sift_up(pq, pq->size - 1);
}

/**
 * Removes and returns the highest priority node from the queue,
 * then restores the heap property.
 * 
 * @param pq Pointer to the priority queue structure.
 * @return The node with the highest priority, or an empty
 * node if the queue is empty.
 */
t_node	pqueue_pop(t_pqueue *pq)
{
	t_node	min;
	t_node	empty;

	if (pq->size == 0)
	{
		empty.coder_id = -1;
		empty.priority = -1;
		return (empty);
	}
	min = pq->heap[0];
	pq->heap[0] = pq->heap[pq->size - 1];
	pq->size--;
	sift_down(pq, 0);
	return (min);
}

/**
 * Returns the highest priority node without removing it from the queue.
 * 
 * @param pq Pointer to the priority queue structure.
 * @return The node at the top of the heap, or an empty node
 * if the queue is empty.
 */
t_node	pqueue_peek(t_pqueue *pq)
{
	t_node	empty;

	if (pq->size == 0)
	{
		empty.coder_id = -1;
		empty.priority = -1;
		return (empty);
	}
	return (pq->heap[0]);
}

/**
 * Deallocates the heap memory and resets the priority queue attributes.
 * 
 * @param pq Pointer to the priority queue structure to be cleared.
 */
void	pqueue_free(t_pqueue *pq)
{
	free(pq->heap);
	pq->heap = NULL;
	pq->size = 0;
	pq->capacity = 0;
}

