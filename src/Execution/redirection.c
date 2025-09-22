#include "minishell.h"
#include "./execution_utils.h"
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

int open_heredoc(char *delim)
{
    char *line;
    int   hd_pipe[2];

    if (pipe(hd_pipe) == -1)
        return (-1);
    while (1)
    {
        line = readline("> ");
        if (!line || streq(line, delim))
        {
            free(line);
            break;
        }
        write(hd_pipe[1], line, slen(line));
        write(hd_pipe[1], "\n", 1);
        free(line);
    }
    close(hd_pipe[1]);
    return (hd_pipe);
}


/*
* Helper function for setup_redirections
*/
static int	handle_input_redirection(t_redir *redir, int *infd)
{
	int	fd;

	if (*infd != -1)
		close(*infd);
	if (redir->type == TOKEN_HEREDOC)
	{
		fd = open_heredoc(redir->target);
		if (fd < 0)
			return (-1);
	}
	else
	{
		fd = open(redir->target, O_RDONLY);
		if (fd < 0)
			return (-1);
	}
	*infd = fd;
	return (0);
}

/*
* Helper function for setup_redirections
*/
static int	handle_output_redirection(t_redir *redir, int *outfd)
{
	int	fd;

	if (*outfd != -1)
		close(*outfd);
	if (redir->type == TOKEN_REDIRECT_OUTPUT_APPEND)
		fd = open(redir->target, O_CREAT | O_WRONLY | O_APPEND, 0644);
	else
		fd = open(redir->target, O_CREAT | O_WRONLY | O_TRUNC, 0644);
	if (fd < 0)
		return (-1);
	*outfd = fd;
	return (0);
}

/*
* Open all redirections for a command, set infd/outfd for process.
* Returns 0 on success, -1 on error.
*/
int	setup_redirections(t_redir *redirs, int *infd, int *outfd)
{
	t_redir	*redir;

	*infd = -1;
	*outfd = -1;
	redir = redirs;
	while (redir)
	{
		if (redir->type == TOKEN_REDIRECT_INPUT || redir->type == TOKEN_HEREDOC)
		{
			if (handle_input_redirection(redir, infd) == -1)
				return (-1);
		}
		else if (redir->type == TOKEN_REDIRECT_OUTPUT || redir->type == TOKEN_REDIRECT_OUTPUT_APPEND)
		{
			if (handle_output_redirection(redir, outfd) == -1)
				return (-1);
		}
		redir = redir->next;
	}
	return (0);
}
