/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sergio-alejandro <sergio-alejandro@stud    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2026/03/21 17:33:52 by sergio-alej       #+#    #+#             */
/*   Updated: 2026/05/26 09:04:22 by sergio-alej      ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "codexion.h"

/**
 * The entry point of the application which initializes the environment,
 * parses input arguments, and manages the simulation lifecycle.
 * 
 * @param argc The number of command-line arguments.
 * @param argv The array of command-line argument strings.
 * @return 0 on successful completion, or 1 if an error occurs during
 * configuration or initialization.
 */
int	main(int argc, char **argv)
{
	t_env	env;

	memset(&env, 0, sizeof(t_env));
	if (parse_config(&env.config, argc, argv))
		return (1);
	if (init_simulation(&env, env.config.number_of_coders))
		return (1);
	start_simulation(&env, env.config.number_of_coders);
	cleanup(&env, env.config.number_of_coders);
	return (0);
}
