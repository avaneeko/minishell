/* ************************************************************************** */
/*                                                                            */
/*                                  executor.c                                */
/*                                                                            */
/*   Implements the simplified “tmp_in + one pipe per step” pipeline model    */
/*   Children set default signals, wire FDs, then call exec_command.          */
/* 																			  */
/*   This file defines:                                                       */
/*     - execute_pipeline                                                     */
/*     - spawn_one_child (static)                                             */
/*     - child_apply_stdin (static)                                           */
/*     - child_apply_stdout (static)                                          */
/*     - count_commands (static)                                              */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"                  /* t_app, t_command, t_env, t_redir */
#include <unistd.h>                     /* pipe, fork, dup2, close, _exit   */
#include <sys/wait.h>                   /* waitpid, W* macros               */
#include <stdlib.h>                     /* malloc, free                     */
#include "execution_utils.h"

void  set_child_signals(void);

/* Duplicate infile or tmp_in to STDIN if present, and close the used FD.  */
static void	child_apply_stdin(int infile, int tmp_in)
{
	/* Prefer explicit infile if provided, otherwise use previous step tmp_in. */
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

/* Duplicate outfile to STDOUT if provided, else use current pipe write end.  */
static void	child_apply_stdout(int outfile, int is_last, int pipe_w)
{
	/* If an explicit outfile exists, prefer it; otherwise use the pipe when not last.  */
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

/* Fork one child, apply redirections, wire stdin/stdout, then exec_command. */
static pid_t	spawn_one_child(
	t_app *app, t_command *cmd, t_env *env, int tmp_in, int pipefd[2], int is_last
)
{
	pid_t	pid;
	int		infd;
	int		outfd;

	infd = -1;
	outfd = -1;
	if (setup_redirections(app, *cmd->redirs, &infd, &outfd) < 0)
		return (-1);
	pid = fork();
	if (pid == 0)
	{
		set_child_signals();
		child_apply_stdin(infd, tmp_in);
		child_apply_stdout(outfd, is_last, pipefd[1]);
		close_if_valid(&pipefd[0]);
		close_if_valid(&pipefd[1]);
		close_if_valid(&infd);
		close_if_valid(&outfd);
		exec_command(cmd, env);
		_exit(126);
	}
	close_if_valid(&infd);
	close_if_valid(&outfd);
	return (pid);
}

/* Count commands in a linked pipeline list. */
static int	count_commands(t_command *head)
{
	int n;

	n = 0;
	while (head)
	{
		n += 1;
		head = head->next;
	}
	return (n);
}

/* Execute a pipeline using one pipe per step and a carried tmp_in read end. */
int	execute_pipeline(t_app *app, t_command *head, t_env *env)
{
	int			n;
	pid_t		*pids;
	int			i;
	int			tmp_in;
	int			pipefd[2];
	t_command	*cur;

	n = count_commands(head);
	if (n <= 0)
		return (0);
	pids = (pid_t *)malloc(sizeof(pid_t) * n);
	if (!pids)
		return (1);
	i = 0;
	tmp_in = -1;
	cur = head;
	while (cur)
	{
		pipefd[0] = -1;
		pipefd[1] = -1;
		if (i != n - 1 && pipe(pipefd) < 0)
		{
			close_if_valid(&tmp_in);
			free(pids);
			return (1);
		}
		pids[i] = spawn_one_child(app, cur, env, tmp_in, pipefd, i == (n - 1));
		if (pids[i] < 0)
		{
			close_pipe_pair(pipefd);
			close_if_valid(&tmp_in);
			free(pids);
			return (1);
		}
		close_if_valid(&pipefd[1]);
		close_if_valid(&tmp_in);
		tmp_in = pipefd[0];
		cur = cur->next;
		i += 1;
	}
	close_if_valid(&tmp_in);
	i = wait_pipeline(pids, n);
	free(pids);
	return (i);
}
