/* ************************************************************************** */
/*                                                                            */
/*                               redirection.c                                */
/*                                                                            */
/*   Opens and wires input/output files and heredocs for a command, returning */
/*   prepared infile/outfile FDs to the caller, as established in EXECUTIONS. */
/*                                                                            */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"                  /* t_redir, token kinds, t_app */
#include <fcntl.h>                      /* open flags                  */
#include <unistd.h>                     /* close / write               */
#include <string.h>						/* strerror */
#include <stdint.h>						/* uintptr_t*/
#include <errno.h>						/* O_* */

/* Print: minishell: <target>: <strerror(errno)>\n */
static void print_open_error(char const *target)
{
    char const *msg;

    write(2, "minishell: ", 11);
    if (target)
        write(2, target, (int)slen(target));
    write(2, ": ", 2);
    msg = strerror(errno);
    if (msg)
        write(2, msg, (int)slen(msg));
    write(2, "\n", 1);
}

/* Handle single input redirection or heredoc, updating infd; print on error. */
int handle_input_redirection(t_app const *app, t_redir redir, int *infd)
{
    int             fd;
    unsigned int    idx;

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

/* Handle single output redirection (truncate or append), updating outfd; print on error. */
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

/* Open all declared redirections for a command and return infd/outfd. */
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
