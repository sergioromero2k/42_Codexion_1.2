/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parsing.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/22 05:23:17 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/05/26 09:03:43 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * Check if a string represents a validate positive number.
 *
 * @param str Input string.
 * @return 1 if valid number, 0 otherwise.
 */

static int	is_valid_number(char *str)
{
	int	i;

	i = 0;
	if (!str[i])
		return (0);
	if (str[0] == '+')
		i++;
	if (!str[i])
		return (0);
	if (strlen(&str[i]) > 5)
		return (0);
	while (str[i])
	{
		if (str[i] < '0' || str[i] > '9')
			return (0);
		i++;
	}
	return (1);
}
/**
 * Parse scheduler type from string and set in config.
 *
 * @param config Pointer to configuration struct.
 * @param str Schduler string ("fifo" or "edf").
 * @return 0 on success, 1 on error.
 */

int	parse_config_2(t_config *config, char *str)
{
	if (config->number_of_coders > 200 || config->time_to_burnout > 100000
		|| config->time_to_compile > 100000 || config->time_to_debug > 100000
		|| config->time_to_refactor > 100000)
		return (write(2, "Error: Argument exceeds safe limits\n", 36), 1);
	if (config->number_of_coders < 1)
		return (write(2, "Error: Coder count\n", 19), 1);
	if (strcmp(str, "fifo") == 0)
		config->scheduler = E_SCHED_FIFO;
	else if (strcmp(str, "edf") == 0)
		config->scheduler = E_SCHED_EDF;
	else
		return (write(2, "Error: Scheduler must be 'fifo' or 'edf'\n", 41), 1);
	return (0);
}

int	parse_config(t_config *config, int argc, char **argv)
{
	int	i;

	if (argc != 9)
		return (write(2, "Error: Wrong number of arguments\n", 33), 1);
	i = 1;
	while (i <= 7)
	{
		if (!is_valid_number(argv[i]))
			return (write(2, "Error: Invalid argument\n", 24), 1);
		i++;
	}
	config->number_of_coders = atoi(argv[1]);
	config->time_to_burnout = atol(argv[2]);
	config->time_to_compile = atol(argv[3]);
	config->time_to_debug = atol(argv[4]);
	config->time_to_refactor = atol(argv[5]);
	config->number_of_compiles_required = atol(argv[6]);
	config->dongle_cooldown = atol(argv[7]);
	return (parse_config_2(config, argv[8]));
}
