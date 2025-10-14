/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils4.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 17:07:57 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/14 17:21:21 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

void	save_stdio(int saved[2])
{
	saved[0] = -1;
	saved[1] = -1;
	saved[0] = dup(STDIN_FILENO);
	saved[1] = dup(STDOUT_FILENO);
}

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

/* Choose stdin in child (infile else tmp_in). */
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

/* Choose stdout in child (outfile else current pipe write). */
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

/* ===================== Parent-builtin fast path ===================== */
/* Parent-builtins are state-changing and must run in the parent when alone. */
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
