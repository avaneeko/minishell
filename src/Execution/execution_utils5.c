/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils5.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 17:14:05 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/15 23:43:21 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

/**
 * @brief Apply open input/output FDs to the parent’s stdio and close originals.
 * @param infd Input FD to dup2 to STDIN, or -1 to skip.
 * @param outfd Output FD to dup2 to STDOUT, or -1 to skip.
 */
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

/**
 * @brief Open and apply redirections in the parent; return 0 on success and 1
 * 		on failure.
 * @param app Application state for heredoc access and errors.
 * @param redirs Linked list of redirections to apply.
 * @return 0 if redirections applied successfully, 1 on setup error.
 */
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

/**
 * @brief Run a single builtin in the parent fast path, storing the status and
 * 		returning 1 to indicate handled.
 * @param app Application context.
 * @param cmd Single command to execute.
 * @param env Environment variables.
 * @param status Output: exit status set by the builtin.
 * @return Always 1 since it handles the execution when called.
 */
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

/**
 * @brief If exactly one parent-only builtin is present, run it in the parent
 * 		with redirections and return 1.
 * @param app Application state.
 * @param cmd Head of the pipeline; must be single command for this path.
 * @param env Environment variables.
 * @param status Output: builtin exit status if handled.
 * @return 1 if executed in parent, 0 otherwise.
 */
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
