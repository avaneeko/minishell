/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils5.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 17:14:05 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/14 17:16:31 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

/* Apply already-opened infd/outfd to stdio in the parent. */
void	parent_apply_redirs(int infd, int outfd)
{
	if (infd >= 0)
	{
		dup2(infd, STDIN_FILENO);
		close(infd);
	}
	if (outfd >= 0)
	{
		dup2(outfd, STDOUT_FILENO);
		close(outfd);
	}
}

/* Helper 1: open and apply redirections in parent; returns 0 ok, 1 on fail */
int	parent_open_and_apply_redirs(t_app *app, t_redir *redirs)
{
	int	infd;
	int	outfd;

	infd = -1;
	outfd = -1;
	if (setup_redirections(app, redirs, &infd, &outfd) < 0)
		return (1);
	parent_apply_redirs(infd, outfd);
	return (0);
}

/* Helper 2: run the single builtin in parent; sets *status and returns 1 */
int	parent_run_single_builtin(t_app *app, t_command *cmd,
										t_env *env, int *status)
{
	if (streq(cmd->argv[0], "exit"))
	{
		*status = builtin_exit_parent(app, cmd->argv);
		return (1);
	}
	*status = exec_builtin(app, cmd->argv, env);
	return (1);
}

/* checks, save/restore stdio, and delegate work */
int	try_run_parent_builtin(t_app *app, t_command *cmd,
									t_env *env, int *status)
{
	int	saved[2];

	if (!cmd || !cmd->argv || !cmd->argv[0])
		return (0);
	if (!is_parent_builtin(cmd->argv[0]) || cmd->next != NULL)
		return (0);
	save_stdio(saved);
	if (parent_open_and_apply_redirs(app, *cmd->redirs))
	{
		restore_stdio(saved);
		return (1);
	}
	if (streq(cmd->argv[0], "exit"))
	{
		restore_stdio(saved);
		return (parent_run_single_builtin(app, cmd, env, status));
	}
	parent_run_single_builtin(app, cmd, env, status);
	restore_stdio(saved);
	return (1);
}
