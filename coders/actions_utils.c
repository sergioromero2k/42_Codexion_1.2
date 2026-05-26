/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions_utils.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/04/03 19:31:35 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/05/26 18:49:10 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Checks if the current coder is at the front of the priority queue
 * for a specific dongle.
 *
 * @param dongle Pointer the structure containing the waiting queue.
 * @param coder_id The unique identifier of the coder checking the queue.
 * @return 1 if it is the coder's turn, 0 otherwise.
 */
int	is_my_turn(t_dongle *dongle, int coder_id)
{
	t_node	nodo;

	if (dongle->waiters.size == 0)
		return (0);
	nodo = pqueue_peek(&dongle->waiters);
	if (nodo.coder_id == coder_id)
		return (1);
	return (0);
}

/**
 * Determines if a dongle is currently in a mandatory cooldown period.
 *
 * @param dongle Pointer to the dongle structure being checked.
 * @return 1 if the cooldown has not yet expired, 0 otherwise.
 *  */
int	cooldown_active(t_dongle *dongle)
{
	long long	currently_time;

	currently_time = get_time_in_ms();
	if (currently_time < dongle->cooldown_until)
		return (1);
	return (0);
}

/**
 * Thread-safe function to log that a coder has successfully acquired a dongle.
 *
 * @param me Pointer to the coder structure performing the action.
 */
void	print_lock(t_coder *me)
{
	pthread_mutex_lock(me->log_lock);
	printf("%lld %d has taken a dongle\n", get_timestamp(me->start_time),
		me->id);
	pthread_mutex_unlock(me->log_lock);
}

/**
 * Handles the logic for acquiring a single dongle,
 * including priority queue management,
 * cooldown checks, and conditional waiting.
 *
 * @param me Pointer to the coder attempting the take the dongle.
 * @param dongle Pointer to the specific dongle to be acquired.
 */
void	take_one_dongles(t_coder *me, t_dongle *dongle)
{
	struct timespec	ts;

	pthread_mutex_lock(&dongle->mutex);
	pqueue_push(&dongle->waiters, me->id, get_priority(me));
	while (dongle->in_use || cooldown_active(dongle) || !is_my_turn(dongle,
			me->id))
	{
		if (cooldown_active(dongle))
		{
			ts = ms_to_timespec(dongle->cooldown_until - get_time_in_ms());
			pthread_cond_timedwait(&dongle->cond, &dongle->mutex, &ts);
		}
		else
			pthread_cond_wait(&dongle->cond, &dongle->mutex);
		if (sim_is_over(me->env))
		{
			pqueue_remove(&dongle->waiters, me->id);
			pthread_mutex_unlock(&dongle->mutex);
			return ;
		}
	}
	dongle->in_use = 1;
	pqueue_pop(&dongle->waiters);
	pthread_mutex_unlock(&dongle->mutex);
	print_lock(me);
}
