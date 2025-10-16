/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils4.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 17:07:57 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 15:39:37 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

/**
 * @brief Save copies of current stdin and stdout into saved[0] and saved[1].
 * @param saved Output array of size 2 to store duplicate descriptors.
 */
void	save_stdio(int saved[2])
{
	saved[0] = -1;
	saved[1] = -1;
	saved[0] = dup(STDIN_FILENO);
	saved[1] = dup(STDOUT_FILENO);
}

/**
 * @brief Restore stdin/stdout from saved descriptors, closing them afterward.
 * @param saved Array previously filled by save_stdio.
 */
void	restore_stdio(int saved[2])
{
	if (saved[0] >= 0)
	{
		dup2(saved[0], STDIN_FILENO);
		close(saved[0]);
		saved[0] = -1;
	}
	if (saved[1] >= 0)
	{
		dup2(saved[1], STDOUT_FILENO);
		close(saved[1]);
		saved[1] = -1;
	}
}

/**
 * @brief Set up the child's stdin from infile if valid, otherwise from tmp_in
 * 		if valid.
 * @param infile Open file for input redirection or -1.
 * @param tmp_in Read end of a previous pipe or -1.
 */
void	child_apply_stdin(int infile, int tmp_in)
{
	if (infile >= 0)
	{
		dup2(infile, STDIN_FILENO);
		close_if_valid(&infile);
	}
	else if (tmp_in >= 0)
	{
		dup2(tmp_in, STDIN_FILENO);
		close_if_valid(&tmp_in);
	}
}

/**
 * @brief Set up the child's stdout from outfile if valid, else from current
 * 		pipe write end if not last.
 * @param outfile Open file for output redirection or -1.
 * @param is_last Non-zero if this is the last command in the pipeline.
 * @param pipe_w Write end of the current pipe or -1.
 */
void	child_apply_stdout(int outfile, int is_last, int pipe_w)
{
	if (outfile >= 0)
	{
		dup2(outfile, STDOUT_FILENO);
		close_if_valid(&outfile);
	}
	else if (!is_last && pipe_w >= 0)
	{
		dup2(pipe_w, STDOUT_FILENO);
		close_if_valid(&pipe_w);
	}
}

/**
 * @brief Return 1 if the builtin must run in the parent process when
 * 		alone (cd, export, unset, exit).
 * @param name Command name string.
 * @return 1 if parent-only builtin, 0 otherwise.
 */
int	is_parent_builtin(char const *name)
{
	if (!name)
		return (0);
	if (streq(name, "cd"))
		return (1);
	if (streq(name, "export"))
		return (1);
	if (streq(name, "unset"))
		return (1);
	if (streq(name, "exit"))
		return (1);
	return (0);
}
