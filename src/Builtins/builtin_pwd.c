/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:20:35 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/13 22:57:16 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/* -------- physical printer (-P) -------- */
static int	print_physical(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		print_str(2, "minishell: pwd: getcwd failed\n");
		return (1);
	}
	print_line(1, cwd);
	free(cwd);
	return (0);
}

/* -------- logical printer (-L default) -------- */
/* If PWD exists, print it even if getcwd would fail (bash logical mode) */
static int	print_logical_or_fallback(t_env *env)
{
	char	*pwd;

	pwd = env_get_value(env, "PWD");
	if (pwd)
	{
		if (print_line(1, pwd))
			return (0);
	}
	return (print_physical());
}

static int	validate_pwd_opt(char c, int *is_physical)
{
	if (c == 'P')
	{
		*is_physical = 1;
		return (0);
	}
	if (c == 'L')
	{
		*is_physical = 0;
		return (0);
	}
	print_str(2, "minishell: pwd: invalid option\n");
	return (2);
}

/* Sets *is_physical=1 if -P seen, 0 for -L (default); returns 2 on invalid. */
static int	parse_pwd_opts(char **argv, int *is_physical)
{
	int	i;
	int	j;
	int	r;

	*is_physical = 0;
	if (!argv || !argv[1] || argv[1][0] != '-')
		return (0);
	i = 1;
	while (argv[i] && argv[i][0] == '-' && argv[i][1])
	{
		j = 1;
		while (argv[i][j])
		{
			r = validate_pwd_opt(argv[i][j], is_physical);
			if (r != 0)
				return (r);
			j += 1;
		}
		break ;
	}
	return (0);
}

/* -------- entry point -------- */
/* Default to logical (-L) to match bash; -P uses getcwd */
int	builtin_pwd(char **argv, t_env *env)
{
	int	is_physical;
	int	parse_status;

	parse_status = parse_pwd_opts(argv, &is_physical);
	if (parse_status != 0)
		return (parse_status);
	if (is_physical)
		return (print_physical());
	return (print_logical_or_fallback(env));
}
