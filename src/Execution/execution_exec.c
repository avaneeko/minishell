/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_exec.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:49:48 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/14 16:54:43 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"                  /* t_command, t_env                   */
#include "execution_utils.h"
#include "Builtins/builtins_utils.h"

/* Has any slash in the string. */
static int	has_slash(char const *s)
{
	int	i;

	if (!s)
		return (0);
	i = 0;
	while (s[i])
	{
		if (s[i] == '/')
			return (1);
		i += 1;
	}
	return (0);
}

/* Print "command not found" and exit 127. */
static void	cmd_not_found(t_app *app, char const *cmd, char **envp)
{
	write(2, "minishell: command not found: ", 30);
	write(2, cmd, (int)slen(cmd));
	write(2, "\n", 1);
	env_free_serialized(envp);
	app_destroy(app);
	exit(127);
}

/* Try execve and exit 126 on error with perror-like message. */
static void	do_exec_or_fail(t_app *app, char const *path, char **argv,
	char **envp)
{
	execve(path, argv, envp);
	write(2, "minishell: exec error: ", 23);
	write(2, path, (int)slen(path));
	write(2, "\n", 1);
	env_free_serialized(envp);
	app_destroy(app);
	exit(126);
}

static void	destroy_exit(t_app *app)
{
	app_destroy(app);
	exit(1);
}

/* Execute one command in the child: builtin or external with PATH lookup. */
void	exec_command(t_app *app, t_command *cmd, t_env *env)
{
	char	**envp;
	char	*path;
	int		status;

	if (!cmd || !cmd->argv || !cmd->argv[0])
	{
		app_destroy(app);
		exit(0);
	}
	if (is_builtin(cmd->argv[0]))
	{
		status = exec_builtin(app, cmd->argv, env);
		app_destroy(app);
		exit(status);
	}
	envp = env_serialize(env);
	if (!envp)
		destroy_exit(app);
	path = find_command_path(cmd->argv[0], env);
	if (!path && has_slash(cmd->argv[0]))
		do_exec_or_fail(app, cmd->argv[0], cmd->argv, envp);
	if (!path)
		cmd_not_found(app, cmd->argv[0], envp);
	do_exec_or_fail(app, path, cmd->argv, envp);
}
