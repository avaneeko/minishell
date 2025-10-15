/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_helpers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 20:06:38 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/15 23:46:18 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

/**
 * @brief Wire stdin/stdout for a command from the legacy pipes matrix using
 * 		its index.
 * @param cmd Command whose FDs are being set, currently unused in this helper.
 * @param pipes Legacy matrix of pipe pairs.
 * @param n_cmd Number of commands in the pipeline.
 * @param idx Zero-based index of the current command.
 */
void	set_pipe_ends(t_command *cmd, int **pipes, int n_cmd, int idx)
{
	int	i;

	(void)cmd;
	i = 0;
	while (i < n_cmd - 1)
	{
		if (i == idx - 1)
			dup2(pipes[i][0], STDIN_FILENO);
		if (i == idx)
			dup2(pipes[i][1], STDOUT_FILENO);
		i += 1;
	}
}

/**
 * @brief Apply command-specific infile/outfile descriptors, then close the
 * 		originals.
 * @param cmd Command with infile/outfile fields to apply.
 */
void	set_redirs(t_command *cmd)
{
	if (cmd->infile != -1)
	{
		dup2(cmd->infile, STDIN_FILENO);
		close(cmd->infile);
	}
	if (cmd->outfile != -1)
	{
		dup2(cmd->outfile, STDOUT_FILENO);
		close(cmd->outfile);
	}
}

/**
 * @brief Parent-side cleanup after fork: close write ends and carry read end
 * 		to tmp_in.
 * @param tmp_in In/out: previous read end is closed and replaced by the
 * 		new read end.
 * @param c Child IO context with created pipe and temp FDs.
 */
void	parent_after_fork(int *tmp_in, t_child_io_ctx *c)
{
	close_if_valid(&c->io[1]);
	close_if_valid(&c->io[2]);
	close_if_valid(&c->pipefd[1]);
	close_if_valid(tmp_in);
	*tmp_in = c->pipefd[0];
}
