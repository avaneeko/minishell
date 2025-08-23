/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 15:36:28 by jgueon            #+#    #+#             */
/*   Updated: 2025/08/23 19:23:56 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/****************************************************************************
 *  Check if command is a builtin, and one to execute the detected built in *
 *																			*
 ****************************************************************************/
int	is_builtin(const char *cmd)
{
	return (
		streq(cmd, "cd")
		|| streq(cmd, "echo")
		|| streq(cmd, "pwd")
		|| streq(cmd, "export")
		|| streq(cmd, "unset")
		|| streq(cmd, "env")
		|| streq(cmd, "exit")
	);
}

int	exec_builtin(char **argv, t_env *env)
{
	if (streq(argv[0], "cd"))
		return (builtin_cd(argv, env));
	 if (streq(argv, "echo"))
        return builtin_echo(argv);
    if (streq(argv, "pwd"))
        return builtin_pwd();
    if (streq(argv, "export"))
        return builtin_export(argv, env);
    if (streq(argv, "unset"))
        return builtin_unset(argv, env);
    if (streq(argv, "env"))
        return builtin_env(env);
    if (streq(argv, "exit"))
        return builtin_exit(argv);
    return (0);
}
