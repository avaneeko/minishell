/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_unset.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:30:33 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/13 22:56:58 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/* Print the standard minishell-style error for invalid identifiers.          */
static void	print_unset_error(char const *arg)
{
	print_str(2, "minishell: unset: `");
	print_str(2, arg);
	print_str(2, "': not a valid identifier\n");
}

/* Validate an unset key: KEY must match [A-Za-z_][A-Za-z0-9_]* strictly.     */
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

/* -------------------------------- entry point ----------------------------- */
/* Iterate args and remove valid keys; print error for invalid ones.          */
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
