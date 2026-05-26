/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/28 09:23:30 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/05/26 18:46:39 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Retrieves the current system time converted into milliseconds.
 *
 * @return Total milliseconds elapsed since the Epoch (1970).
 */
long long	get_time_in_ms(void)
{
	struct timeval	tv;

	if (gettimeofday(&tv, NULL) == -1)
	{
		write(2, "gettimeofday error\n", 19);
		return (0);
	}
	return ((long long)(tv.tv_sec * 1000) + (tv.tv_usec / 1000));
}

/**
 * Calculates how much has passed since the simulation started.
 *
 * @param start_time The recorded start time of the simulation.
 * @return Elapsed time in milliseconds.
 */
long long	get_timestamp(long long start_time)
{
	return (get_time_in_ms() - start_time);
}

/**
 * Converts a millisecond duration int oan absolute timespec struct
 * for use in timed thread operations.
 *
 * @param ms The amount of milliseconds to add the current time.
 * @return A timespec struct representing the future absolute time.
 */
struct timespec	ms_to_timespec(long long ms)
{
	struct timeval	tv;
	struct timespec	ts;
	long long		total_nsecs;

	gettimeofday(&tv, NULL);
	total_nsecs = (tv.tv_usec * 1000) + ((ms % 1000) * 1000000);
	ts.tv_sec = tv.tv_sec + (ms / 1000) + (total_nsecs / 1000000000);
	ts.tv_nsec = total_nsecs % 1000000000;
	return (ts);
}

/**
 * Calculates the scheduling priority of a coder based on the selected
 * algorithm (FIFO or EDF).
 *
 * @param me Pointer to the coder's individual structure.

	* @return The priority value (lower values 
	typically represent higher priority).
 */
long long	get_priority(t_coder *me)
{
	if (me->config->scheduler == E_SCHED_FIFO)
		return (get_time_in_ms());
	if (me->config->scheduler == E_SCHED_EDF)
		return (me->last_compile_time + me->config->time_to_burnout);
	return (get_time_in_ms());
}
