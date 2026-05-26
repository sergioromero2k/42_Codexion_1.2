/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   routine.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 14:07:21 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/05/26 18:45:46 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Safely updates the glboal simulation state to signal that the simulation
 * should stop.
 *
 * @param env Pointer to the main environment structure.
 * @return THe updated simulation status (1).
 */
int	set_sim_over(t_env *env)
{
	int	result;

	pthread_mutex_lock(&env->state_lock);
	env->simulation_over = 1;
	result = env->simulation_over;
	pthread_mutex_unlock(&env->state_lock);
	return (result);
}

/**
 * Thread-safely checks the global enviroment state to determine
 * if the simulaton should terminate.
 *
 * @param env Pointer to the main environment structure.
 * @return 1 if the simulation has ended, 0 otherwise.
 */
int	sim_is_over(t_env *env)
{
	int	simulation;

	pthread_mutex_lock(&env->state_lock);
	simulation = env->simulation_over;
	pthread_mutex_unlock(&env->state_lock);
	return (simulation);
}

/**
 * Main routine for each coder thread.
 *
 * @param arg Type-casted pointer to the specific t_coder structure.
 * @return Always returns NULL upon thread exit.
 */
void	*coder_routine(void *arg)
{
	t_coder	*me;

	me = (t_coder *)arg;
	if (me->id % 2 == 0)
		usleep(1500);
	while (!(sim_is_over(me->env)))
	{
		take_dongles(me);
		if (sim_is_over(me->env))
		{
			drop_dongles(me);
			break ;
		}
		coder_compile(me);
		drop_dongles(me);
		if (sim_is_over(me->env))
			break ;
		coder_debug(me);
		if (sim_is_over(me->env))
			break ;
		coder_refactor(me);
	}
	return (NULL);
}
