#include "minishell.h"
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

/*
* Close fds if not -1.
*/
void	close_redir_fds(int *in, int *out)
{
	if (*in != -1)
	{
		close(*in);
		*in = -1;
	}
	if (*out != -1)
	{
		close(*out);
		*out = -1;
	}
}

/*
* Handle heredoc: Write user input until delimiter,
* return fd ready to read, or -1 on error.
*/
int	open_heredoc(char *delim)
{
	char	*line;
	int		hd_pipe[2];

	if (pipe(hd_pipe) == -1)
	{
		return (-1);
	}
	while (1)
	{
		write(1, "> ", 2);
		line = readline(NULL);
		if (!line || streq(line, delim))
		{
			free(line);
			break ;
		}
		write(hd_pipe[1], line, slen(line));
		write(hd_pipe[1], "\n", 1);
		free(line);
	}
	close(hd_pipe[1]);
	return (hd_pipe);
}

/*
* Open all redirections for a command, set infd/outfd for process.
* Returns 0 on success, -1 on error.
*/
int	setup_redirections(t_redir *redirs, int *infd, int *outfd)
{
	int		fd;
	t_redir	*r;

	*infd = -1;
	*outfd = -1;
	r = redirs;
	while (r)
	{
		if (r->type == TOKEN_REDIRECT_INPUT)
		{
			if (*infd != -1)
				close(*infd);
			fd = open(r->target, O_RDONLY);
			if (fd < 0)
				return (-1);
			*infd = fd;
		}
		else if (r->type == TOKEN_REDIRECT_OUTPUT)
		{
			if (*outfd != -1)
				close(*outfd);
			fd = open(r->target, O_CREAT | O_WRONLY | O_TRUNC, 0644);
			if (fd < 0)
				return (-1);
			*outfd = fd;
		}
		else if (r->type == TOKEN_REDIRECT_OUTPUT_APPEND)
		{
			if (*outfd != -1)
				close(*outfd);
			fd = open(r->target, O_CREAT | O_WRONLY | O_APPEND, 0644);
			if (fd < 0)
				return (-1);
			*outfd = fd;
		}
		else if (r->type == TOKEN_HEREDOC)
		{
			if (*infd != -1)
				close(*infd);
			fd = open_heredoc(r->target);
			if (fd < 0)
				return (-1);
			*infd = fd;
		}
		r = r->next;
	}
	return (0);
}
