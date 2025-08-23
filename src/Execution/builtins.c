/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 15:36:28 by jgueon            #+#    #+#             */
/*   Updated: 2025/08/23 17:09:21 by jgueon           ###   ########.fr       */
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

// change directory
int	builtin_cd(char **argv, t_env *env)
{
	(void)argv;
	(void)env;
	//TO DO: implement cd logic
	return (0);
}

// Print arguments
int	builtin_echo(char **argv)
{
	(void)argv;
	// TODO: implement echo logic
	return (0);
}

// Print arguments
int	builtin_echo(char **argv)
{
	(void)argv;
	// TODO: implement echo logic
	return (0);
}

// Print the current working directory
int	builtin_pwd(void)
{
	//TODO: implement pwd logic
	return (0);
}

// Set or display environment variables
int	builtin_export(char **argv, t_env *env)
{
	(void)argv;
	(void)env;
	//TODO: implement unset logic
	return (0);
}

// Unset environment variables
int	builtin_unset(char **argv, t_env *env)
{
	(void)argv;
	(void)env;
	//TODO: implement unset logic
	return (0);
}

// Print environment variables
int	builtin_env(t_env *env)
{
	(void)env;
	//TODO: implement env logic
	return (0);
}

// Exit the shell
int	builtin_exit(char **argv)
{
	(void)argv;
	// TODO: implement exit logic
	return (0);
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
