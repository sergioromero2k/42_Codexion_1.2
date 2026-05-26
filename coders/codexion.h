/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   codexion.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 20:47:22 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/05/26 08:33:49 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CODEXION_H
# define CODEXION_H

# include <ctype.h>
# include <limits.h>
# include <pthread.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <sys/time.h>
# include <time.h>
# include <unistd.h>

typedef enum e_schedule
{
	E_SCHED_FIFO,
	E_SCHED_EDF,
}					t_schedule;

/**
 * Global configuration settings fot the simulation.
 */
typedef struct s_config
{
	int				number_of_coders;
	long			time_to_burnout;
	long			time_to_compile;
	long			time_to_debug;
	long			time_to_refactor;
	long			number_of_compiles_required;
	long			dongle_cooldown;
	t_schedule		scheduler;
}					t_config;

/**
 * A single node within the priority queue.
 */
typedef struct s_node
{
	int				coder_id;
	long long		priority;
}					t_node;

/**
 * Priority queue structure using a heap for efficient scheduling.
 */
typedef struct s_pqueue
{
	t_node			*heap;
	int				size;
	int				capacity;
}					t_pqueue;

/**
 * Represents a shared resource (dongle) protected by a nutex and condition.
 */
typedef struct s_dongle
{
	pthread_mutex_t	mutex;
	pthread_cond_t	cond;
	int				in_use;
	long			cooldown_until;
	t_pqueue		waiters;
}					t_dongle;

/**
 * Structure representing a coder thread and its local state.
 */
typedef struct s_coder
{
	int				id;
	pthread_t		thread;
	int				compile_count;
	long long		last_compile_time;
	pthread_mutex_t	state_lock;
	long long		start_time;
	t_dongle		*left_dongle;
	t_dongle		*right_dongle;
	pthread_mutex_t	*log_lock;
	t_config		*config;
	struct s_env	*env;
}					t_coder;

/**
 * Main environemnt structure holding all global simulation data.
 */
typedef struct s_env
{
	t_config		config;
	t_coder			*coders;
	t_dongle		*dongles;
	pthread_mutex_t	log_lock;
	pthread_mutex_t	state_lock;
	pthread_t		monitor;
	long long		start_time;
	int				simulation_over;
}					t_env;

/* INITIALIZATION & CLEANUP */

int					parse_config(t_config *config, int argc, char **argv);
int					parse_config_2(t_config *config, char *str);
static int			init_structures(t_env *env, int n);
int					init_simulation(t_env *env, int n);
void				cleanup(t_env *env, int i);

/* SIMULATION CONTROL */

void				start_simulation(t_env *env, int n);
void				*coder_routine(void *arg);
void				*monitor_routine(void *arg);
int					set_sim_over(t_env *env);
int					sim_is_over(t_env *env);

/* CODER ACTIONS */

void				take_dongles(t_coder *me);
void				take_one_dongles(t_coder *me, t_dongle *dongle);
void				drop_dongles(t_coder *me);
void				coder_compile(t_coder *me);
void				coder_debug(t_coder *me);
void				coder_refactor(t_coder *me);

/* MONITORING & HEALTH */

long long			check_health(t_coder *coder);
int					check_all_compiled(t_env *env);
void				print_lock(t_coder *me);

/* TIME & SCHEDULING */

long long			get_time_in_ms(void);
long long			get_timestamp(long long start_time);
long long			get_priority(t_coder *me);
struct timespec		ms_to_timespec(long long ms);
int					is_my_turn(t_dongle *dongle, int coder_id);
int					cooldown_active(t_dongle *dongle);

/* PRIORITY QUEUE */

int					pqueue_init(t_pqueue *pq, int capacity);
void				pqueue_push(t_pqueue *pq, int coder_id, long long priority);
t_node				pqueue_pop(t_pqueue *pq);
t_node				pqueue_peek(t_pqueue *pq);
void				pqueue_free(t_pqueue *pq);
void				sift_up(t_pqueue *pq, int index);
void				sift_down(t_pqueue *pq, int index);

#endif