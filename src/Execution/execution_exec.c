/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_exec.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:49:48 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/15 23:18:11 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"
#include "Builtins/builtins_utils.h"

/**
 * @brief Return 1 if the string contains any slash character '/', else 0.
 * @param s Input string to scan.
 * @return 1 if a slash is present, 0 otherwise.
 */
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

/**
 * @brief Print a "command not found" message and exit the child with
 * 		status 127, cleaning up resources.
 * @param app Application handle to destroy before exiting.
 * @param cmd The command string to report.
 * @param envp Serialized environment to free before exit.
 */
static void	cmd_not_found(t_app *app, char const *cmd, char **envp)
{
	write(2, "minishell: command not found: ", 30);
	write(2, cmd, (int)slen(cmd));
	write(2, "\n", 1);
	env_free_serialized(envp);
	app_destroy(app);
	exit(127);
}

/**
 * @brief Attempt execve on the given path and exit with 126 on failure after
 * 		printing an error.
 * @param app Application handle to destroy on failure.
 * @param path Absolute or relative path to execute.
 * @param argv Argument vector for the new program.
 * @param envp Serialized environment for the new program.
 */
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

/**
 * @brief Destroy the application and exit the process with status 1.
 * @param app Application handle to destroy.
 */
static void	destroy_exit(t_app *app)
{
	app_destroy(app);
	exit(1);
}

/**
 * @brief Execute a single command in the child, handling builtins or external
 * 		commands with PATH lookup.
 * @param app Application context for cleanup and state.
 * @param cmd Parsed command containing argv and redirections.
 * @param env Environment variables in internal form.
 * @return This function does not return on success; it exits the child with
 * 		the appropriate status.
 */
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
