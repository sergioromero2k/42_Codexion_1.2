/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   test_coders.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/02 05:53:20 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/04/02 05:54:16 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

int main(void)
{
    t_pqueue pq;
    
    pqueue_init(&pq, 5);
	pqueue_push(&pq, 3, 500);
	pqueue_push(&pq, 1, 100);
	pqueue_push(&pq, 4, 800);
	pqueue_push(&pq, 2, 300);
    printf("%d\n", pqueue_pop(&pq).coder_id);
	printf("%d\n", pqueue_pop(&pq).coder_id);
	printf("%d\n", pqueue_pop(&pq).coder_id);
	printf("%d\n", pqueue_pop(&pq).coder_id);
	pqueue_free(&pq);
	return (0);

}