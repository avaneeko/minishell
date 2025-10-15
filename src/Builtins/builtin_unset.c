/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:30:33 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 00:48:46 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Print minishell-style error for an invalid unset identifier.
 * @param arg The offending argument to display in the error.
 */
static void	print_unset_error(char const *arg)
{
	print_str(2, "minishell: unset: `");
	print_str(2, arg);
	print_str(2, "': not a valid identifier\n");
}

/**
 * @brief Validate an unset key against [A-Za-z_][A-Za-z0-9_]*.
 * @param s Key string to validate.
 * @return 1 if valid, 0 otherwise.
 */
static int	is_valid_unset_key(char const *s)
{
	int	i;

	if (!s || !s[0])
		return (0);
	if (!is_name_start(s[0]))
		return (0);
	i = 1;
	while (s[i])
	{
		if (!is_name_char(s[i]))
			return (0);
		i += 1;
	}
	return (1);
}

/**
 * @brief Remove variables specified in argv from the environment, reporting
 * 		invalid identifiers.
 * @param argv Arguments where each item is a key to unset.
 * @param env Environment to modify.
 * @return 0 if all keys were valid, 1 if any invalid identifiers were
 * 		encountered.
 */
int	builtin_unset(char **argv, t_env *env)
{
	int	i;
	int	status;

	if (!argv || !argv[0] || !env)
		return (0);
	if (!argv[1])
		return (0);
	i = 1;
	status = 0;
	while (argv[i])
	{
		if (!is_valid_unset_key(argv[i]))
		{
			print_unset_error(argv[i]);
			status = 1;
		}
		else
			remove_epair_by_key(env, argv[i]);
		i += 1;
	}
	return (status);
}
