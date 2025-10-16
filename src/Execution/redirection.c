/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   redirection.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 20:16:09 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/15 23:50:23 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

/**
 * @brief Print a standard open error "minishell: <target>: <strerror>" line
 * 		to stderr.
 * @param target The filename attempted to open, may be NULL.
 */
static void	print_open_error(char const *target)
{
	char const	*msg;

	write(2, "minishell: ", 11);
	if (target)
		write(2, target, (int)slen(target));
	write(2, ": ", 2);
	msg = strerror(errno);
	if (msg)
		write(2, msg, (int)slen(msg));
	write(2, "\n", 1);
}

/**
 * @brief Handle one input redirection or heredoc, updating infd and reporting
 * 		errors.
 * @param app Application context with heredoc array.
 * @param redir A single input-type redirection node.
 * @param infd In/out: previous FD closed and replaced by the new one.
 * @return 0 on success, -1 on failure.
 */
int	handle_input_redirection(t_app const *app, t_redir redir, int *infd)
{
	int				fd;
	unsigned int	idx;

	if (*infd != -1)
	{
		close(*infd);
		*infd = -1;
	}
	if (redir.type == TOKEN_HEREDOC)
	{
		idx = (unsigned int)(uintptr_t)redir.target;
		fd = app->heredocs[idx];
	}
	else
	{
		fd = open(redir.target, O_RDONLY);
		if (fd < 0)
		{
			print_open_error(redir.target);
			return (-1);
		}
	}
	*infd = fd;
	return (0);
}

/**
 * @brief Handle one output redirection (truncate or append), updating outfd and
 * 		reporting errors.
 * @param redir A single output-type redirection node.
 * @param outfd In/out: previous FD closed and replaced by the new one.
 * @return 0 on success, -1 on failure.
 */
static int	handle_output_redirection(t_redir redir, int *outfd)
{
	int	fd;

	if (*outfd != -1)
	{
		close(*outfd);
		*outfd = -1;
	}
	if (redir.type == TOKEN_REDIRECT_OUTPUT_APPEND)
		fd = open(redir.target, O_CREAT | O_WRONLY | O_APPEND, 0644);
	else
		fd = open(redir.target, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
	{
		print_open_error(redir.target);
		return (-1);
	}
	*outfd = fd;
	return (0);
}

/**
 * @brief Open all redirections in a command and return the resulting infd and
 * 		outfd to apply.
 * @param app Application context for heredoc descriptors.
 * @param redirs Linked list of redirection nodes.
 * @param infd Output: final input FD or -1.
 * @param outfd Output: final output FD or -1.
 * @return 0 on success, -1 on any redirection failure.
 */
int	setup_redirections(t_app *app, t_redir *redirs, int *infd, int *outfd)
{
	t_redir	*rp;

	*infd = -1;
	*outfd = -1;
	rp = redirs;
	while (rp)
	{
		if (rp->type == TOKEN_REDIRECT_INPUT || rp->type == TOKEN_HEREDOC)
		{
			if (handle_input_redirection(app, *rp, infd) < 0)
				return (-1);
		}
		else if (rp->type == TOKEN_REDIRECT_OUTPUT
			|| rp->type == TOKEN_REDIRECT_OUTPUT_APPEND)
		{
			if (handle_output_redirection(*rp, outfd) < 0)
				return (-1);
		}
		rp = rp->next;
	}
	return (0);
}
