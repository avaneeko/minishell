/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_helpers.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 20:06:38 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/09 20:06:57 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>                     /* dup2, close   */
#include "minishell.h"                  /* t_command     */

/* ************************************************************************** */
/*                                                                            */
/*                             executor_helpers.c                             */
/*                                                                            */
/*   Legacy helpers used by the previous matrix-pipes implementation; kept so */
/*   existing code that references them still compiles and behaves the same.  */
/*                                                                            */
/* ************************************************************************** */
/* Wire stdin/stdout from the legacy pipes matrix for command idx. */
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

/* Apply cmd->infile/outfile when set, then close the originals. */
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
