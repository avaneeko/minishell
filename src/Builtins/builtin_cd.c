/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:01:30 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/07 21:58:47 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"
#include <unistd.h>   // chdir, getcwd, write
#include <stdlib.h>   // getenv
#include <stdio.h>    // perror
#include <linux/limits.h>   // PATH_MAX

static int	resolve_target(char **argv, char **out)
{
	char	*home;

	*out = 0;
	if (argv && argv[1])
	{
		*out = argv[1];
		return (1);
	}
	home = getenv("HOME");
	if (!home)
	{
		write(2, "cd: HOME not set\n", 17);
		return (0);
	}
	*out = home;
	return (1);
}

static char	*fetch_oldpwd(t_env *env)
{
	t_epair	pair;

	if (get_epair_by_key(env, "PWD", &pair))
		return (pair.value);
	return (0);
}

static int	capture_cwd(char *buf, size_t size)
{
	if (!getcwd(buf, size))
	{
		perror("cd");
		return (0);
	}
	return (1);
}

static void	update_pwd_vars(t_env *env, char *oldpwd, char *cwd)
{
	if (!oldpwd)
		oldpwd = "";
	env_set(env, "OLDPWD", oldpwd, ORIGIN_ENV);
	env_set(env, "PWD", cwd, ORIGIN_ENV);
}

int builtin_cd(char **argv, t_env *env)
{
    char *target;
    char cwd[PATH_MAX];
    char *oldpwd;

    if (argv[1] && argv[2])
    {
        write(2, "minishell: cd: too many arguments\n", 34);
        return (1);
	}
    target = 0;
    if (!resolve_target(argv, &target))
        return (1);
    oldpwd = fetch_oldpwd(env);
    if (chdir(target) != 0)
    {
        perror("minishell: cd");
        return (1);
    }
    if (!capture_cwd(cwd, sizeof(cwd)))
        return (1);
    update_pwd_vars(env, oldpwd, cwd);
    return (0);
}

