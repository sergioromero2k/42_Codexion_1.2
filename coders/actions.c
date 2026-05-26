/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   actions.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 20:04:59 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/05/26 08:48:45 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Attempts to acquire both the right and left dongles
 * in sequence to begin compilation.
 *
 * @param me Pointer to the coder's individual structure.
 */
void	take_dongles(t_coder *me)
{
	if (me->config->number_of_coders == 1)
	{
		take_one_dongles(me, me->right_dongle);
		return ;
	}
	take_one_dongles(me, me->right_dongle);
	if (sim_is_over(me->env))
		return ;
	take_one_dongles(me, me->left_dongle);
}

/**
 * Releases both held dongles, sets their cooldown period,
 * and signals any waiting threads.
 *
 * @param me Pointer to th e coder's individual structure.
 */
void	drop_dongles(t_coder *me)
{
	pthread_mutex_lock(&me->right_dongle->mutex);
	me->right_dongle->in_use = 0;
	me->right_dongle->cooldown_until = get_time_in_ms()
		+ me->config->dongle_cooldown;
	pthread_cond_broadcast(&me->right_dongle->cond);
	pthread_mutex_unlock(&me->right_dongle->mutex);
	if (me->config->number_of_coders == 1)
		return ;
	pthread_mutex_lock(&me->left_dongle->mutex);
	me->left_dongle->in_use = 0;
	me->left_dongle->cooldown_until = get_time_in_ms()
		+ me->config->dongle_cooldown;
	pthread_cond_broadcast(&me->left_dongle->cond);
	pthread_mutex_unlock(&me->left_dongle->mutex);
}

/**
 *Handles the compilation phase, updating the coder's internal state
 * and logging the activity.
 *
 * @param me Pointer to the coder's individual structure.
 */
void	coder_compile(t_coder *me)
{
	pthread_mutex_lock(&me->state_lock);
	me->last_compile_time = get_timestamp(me->start_time);
	pthread_mutex_unlock(&me->state_lock);
	pthread_mutex_lock(me->log_lock);
	printf("%lld %d is compiling\n", get_timestamp(me->start_time), me->id);
	pthread_mutex_unlock(me->log_lock);
	usleep(me->config->time_to_compile * 1000);
	pthread_mutex_lock(&me->state_lock);
	me->compile_count++;
	pthread_mutex_unlock(&me->state_lock);
}

/**
 * Logs the debugging status and pauses execution for the configured duration.
 *
 * @param me Pointer to the coder's individual structure.
 */
void	coder_debug(t_coder *me)
{
	pthread_mutex_lock(me->log_lock);
	printf("%lld %d is debugging\n", get_timestamp(me->start_time), me->id);
	pthread_mutex_unlock(me->log_lock);
	usleep(me->config->time_to_debug * 1000);
}

/**
 * Simulates the refactoring phase by logging the action and pausing the thread.
 *
 * @param me Pointer to the coder's individual structure.
 */
void	coder_refactor(t_coder *me)
{
	pthread_mutex_lock(me->log_lock);
	printf("%lld %d is refactoring\n", get_timestamp(me->start_time), me->id);
	pthread_mutex_unlock(me->log_lock);
	usleep(me->config->time_to_refactor * 1000);
}
