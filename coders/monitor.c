/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   monitor.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/30 14:07:27 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/05/26 08:47:33 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Calculates the time elapsed since the coder's last successful
 * compilation to monitor for potential burnout.
 *
 * @param coder Pointer to the coder structure being checked.
 * @return Elapsed time in milliseconds since the last compilation.
 */
long long	check_health(t_coder *coder)
{
	long long	elapsed;

	pthread_mutex_lock(&coder->state_lock);
	elapsed = get_time_in_ms() - coder->last_compile_time;
	pthread_mutex_unlock(&coder->state_lock);
	return (elapsed);
}

/**
 * Verifies if every coder in the simulatiom has reached the required number of
 * successful compilations.
 *
 * @param env Pointer to the main environment structure.
 * @return 1 if all coders have finished their tasks, 0 otherwise.
 */
int	check_all_compiled(t_env *env)
{
	int	i;
	int	compile;

	i = 0;
	while (i < (env->config.number_of_coders))
	{
		pthread_mutex_lock(&env->coders[i].state_lock);
		compile = env->coders[i].compile_count;
		pthread_mutex_unlock(&env->coders[i].state_lock);
		if (compile < (env->config.number_of_compiles_required))
			return (0);
		i++;
	}
	return (1);
}

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
 * Manages the termination process when a coder fails,
 * logging the burnout and waking up all threads to exit.
 *
 * @param env Pointer to the main environment structure.
 * @param i Index of the coder who triggered the burnout.
 */
static void	handle_burnout(t_env *env, int i)
{
	int	j;

	pthread_mutex_lock(&env->log_lock);
	printf("%lld %d burned out\n", get_timestamp(env->start_time),
		env->coders[i].id);
	pthread_mutex_unlock(&env->log_lock);
	set_sim_over(env);
	j = 0;
	while (j < env->config.number_of_coders)
	{
		pthread_mutex_lock(&env->dongles[j].mutex);
		pthread_cond_broadcast(&env->dongles[j].cond);
		pthread_mutex_unlock(&env->dongles[j].mutex);
		j++;
	}
}

/**
 * The central observer thread that continuously patrols
 * coders to check for burnout conditions or goal completion.
 *
 * @param arg Generic pointer to the global t_env structure.
 * @return NULL when the simulation reaches a termination state.
 */
void	*monitor_routine(void *arg)
{
	t_env	*env;
	int		i;

	env = (t_env *)arg;
	while (!is_simulation_over(env))
	{
		i = 0;
		while (i < env->config.number_of_coders)
		{
			if (check_health(&env->coders[i]) >= env->config.time_to_burnout)
				return (handle_burnout(env, i), NULL);
			i++;
		}
		if (check_all_compiled(env))
		{
			set_sim_over(env);
			return (NULL);
		}
		usleep(500);
	}
	return (NULL);
}
