/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 22:22:36 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/10 21:06:51 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


#include "minishell.h"
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "../Builtins/builtins_utils.h"

// =========================================================================  /
/* ************************************************************************** */
/*                                                                            */
/*                                 executor.c                                 */
/*                                                                            */
/*   Purpose:                                                                 */
/*   - Execute pipelines with one new pipe per step and a carried tmp_in.     */
/*   - Run state-changing builtins (cd/export/unset/exit) in parent when      */
/*     there is a single command (no pipe), so changes persist.               */
/*                                                                            */
/*                                                                            */
/*   External helpers (provided elsewhere):                                   */
/*     int   setup_redirections(t_app*, t_redir*, int*, int*);                */
/*     void  set_child_signals(void);                                         */
/*     void  exec_command(t_command*, t_env*);                                */
/*     int   wait_pipeline(pid_t*, int);                                      */
/*     void  close_if_valid(int*);                                            */
/*     int   exec_builtin(char **argv, t_env *env);                           */
/*     int   streq(char const *a, char const *b);                             */
/*                                                                            */
/* ************************************************************************** */

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

// ADDED 8th OCTOBER for fd open issue
static void	save_stdio(int saved[2])
{
	saved[0] = -1;
	saved[1] = -1;
	saved[0] = dup(STDIN_FILENO);
	saved[1] = dup(STDOUT_FILENO);
}

// ADDED 8th OCTOBER for fd open issue
void restore_stdio(int saved[2])
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
static void	child_apply_stdin(int infile, int tmp_in)
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
static void	child_apply_stdout(int outfile, int is_last, int pipe_w)
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
static int	is_parent_builtin(char const *name)
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

/* Apply already-opened infd/outfd to stdio in the parent. */
static void	parent_apply_redirs(int infd, int outfd)
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


/* Try to handle a single parent-builtin; returns 1 if handled (status set). */
// static int	try_run_parent_builtin(t_app *app, t_command *cmd, t_env *env, int *status)
// {
// 	int saved[2];
// 	int infd;
// 	int outfd;

// 	if (!cmd || !cmd->argv || !cmd->argv[0])
// 		return (0);
// 	if (!is_parent_builtin(cmd->argv[0]) || cmd->next != NULL)
// 		return (0);
// 	save_stdio(saved);
// 	infd = -1;
// 	outfd = -1;
// 	if (setup_redirections(app, *cmd->redirs, &infd, &outfd) < 0)
// 	{
// 		restore_stdio(saved);
// 		return (1);
// 	}
// 	parent_apply_redirs(infd, outfd);
// 	if (streq(cmd->argv[0], "exit"))		// ADDED 7.10
// 	{
// 		*status = builtin_exit_parent(app, cmd->argv);		//ADDED 7.10
// 		restore_stdio(saved);
// 		return (1);

// 	}
// 	*status = exec_builtin(app, cmd->argv, env);
// 	restore_stdio(saved);
// 	return (1);
// }
// ADDED 8th OCTOBER for fd open issue
/* Handle single parent-builtin; returns 1 if handled (status set). */
// static int	try_run_parent_builtin(t_app *app, t_command *cmd, t_env *env, int *status)
// {
// 	int saved[2];
// 	int infd;
// 	int outfd;

// 	if (!cmd || !cmd->argv || !cmd->argv[0])
// 		return (0);
// 	if (!is_parent_builtin(cmd->argv[0]) || cmd->next != NULL)
// 		return (0);
// 	save_stdio(saved);
// 	infd = -1;
// 	outfd = -1;
// 	if (setup_redirections(app, *cmd->redirs, &infd, &outfd) < 0)
// 	{
// 		restore_stdio(saved);
// 		return (1);
// 	}
// 	parent_apply_redirs(infd, outfd);
// 	if (streq(cmd->argv[0], "exit"))
// 	{
// 		/* Critical: restore stdio BEFORE a builtin that may exit(). */
// 		restore_stdio(saved);
// 		// app_reset_exec(app); //ADDED 08.10 FOR: Leak in parent
// 		*status = builtin_exit_parent(app, cmd->argv);
// 		return (1);
// 	}
// 	*status = exec_builtin(app, cmd->argv, env);
// 	restore_stdio(saved);
// 	return (1);
// }



// /* ====================== One-step child lifecycle ====================== */

// /* Prepare one step: set is_last, create pipe if needed, open redirs. */
// /* prepare_step — do not open redirs here; only set up pipes and flags        */
// static int	prepare_step(t_app *app, t_command *cmd, int pipefd[2], int io[4])
// {
// 	(void)app;
// 	io[1] = -1;
// 	io[2] = -1;
// 	io[3] = 0;
// 	if (!cmd->next)
// 		io[3] = 1;
// 	pipefd[0] = -1;
// 	pipefd[1] = -1;
// 	if (!io[3] && pipe(pipefd) < 0)
// 		return (-1);
// 	return (0);
// }

// /* Child code path: set signals, wire FDs, and exec command. */
// static void	child_exec(t_app *app, t_command *cmd, t_env *env,
//                        int pipefd[2], int io[4])
// {
// 	int infd;
// 	int outfd;

// 	set_child_signals();
// 	infd = -1;
// 	outfd = -1;
// 	if (setup_redirections(app, *cmd->redirs, &infd, &outfd) < 0)
// 	{
// 		close_if_valid(&pipefd[0]);
// 		close_if_valid(&pipefd[1]);
// 		_exit(1);
// 	}
// 	child_apply_stdin(infd, io[0]);
// 	child_apply_stdout(outfd, io[3], pipefd[1]);
// 	close_if_valid(&pipefd[0]);
// 	close_if_valid(&pipefd[1]);
// 	close_if_valid(&infd);
// 	close_if_valid(&outfd);
// 	exec_command(app, cmd, env);
// 	_exit(126);
// }

// /* Parent after fork: close write, close old tmp_in, carry read end. */
// static void	parent_after_fork(int *tmp_in, int pipefd[2], int io[4])
// {
// 	close_if_valid(&io[1]);
// 	close_if_valid(&io[2]);
// 	close_if_valid(&pipefd[1]);
// 	close_if_valid(tmp_in);
// 	*tmp_in = pipefd[0];
// }

// /* One pipeline step: open/pipe, fork, child/parent split, return pid or -1. */
// static pid_t	step_setup_and_fork(t_app *app, t_command *cmd, t_env *env, int *tmp_in)
// {
// 	int		pipefd[2];
// 	int		io[4];
// 	pid_t	pid;

// 	io[0] = *tmp_in;
// 	if (prepare_step(app, cmd, pipefd, io) < 0)
// 		return (-1);
// 	pid = fork();
// 	if (pid == 0)
// 		child_exec(app, cmd, env, pipefd, io);
// 	parent_after_fork(tmp_in, pipefd, io);
// 	return (pid);
// }

// /* ========================= Pipeline orchestrator ========================= */

// /* Run all steps; return last status or 1 on early failure. */
// static int	pipeline_run(t_app *app, t_env *env, t_command *head, pid_t *pids)
// {
// 	int			i;
// 	int			tmp_in;
// 	t_command	*cur;
// 	pid_t		pid;

// 	i = 0;
// 	tmp_in = -1;
// 	cur = head;
// 	while (cur)
// 	{
// 		pid = step_setup_and_fork(app, cur, env, &tmp_in);
// 		if (pid < 0)
// 		{
// 			close_if_valid(&tmp_in);
// 			return (1);
// 		}
// 		pids[i] = pid;
// 		cur = cur->next;
// 		i += 1;
// 	}
// 	close_if_valid(&tmp_in);
// 	return (wait_pipeline(pids, i));
// }

// /* Public entry: parent-builtin fast path, else forked pipeline. */
// int	execute_pipeline(t_app *app, t_command *head, t_env *env)
// {
// 	pid_t	*pids;
// 	int		n;
// 	int		code;

// 	if (!head)
// 		return (0);
// 	if (!head->next && head->argv && head->argv[0])
// 	{
// 		if (try_run_parent_builtin(app, head, env, &code))
// 			return (code);
// 	}
// 	n = count_commands(head);
// 	if (n <= 0)
// 		return (0);
// 	pids = (pid_t *)malloc(sizeof(pid_t) * n);
// 	if (!pids)
// 		return (1);
// 	code = pipeline_run(app, env, head, pids);
// 	free(pids);
// 	return (code);
// }


/* Helper 1: open and apply redirections in parent; returns 0 ok, 1 on fail */
static int	parent_open_and_apply_redirs(t_app *app, t_redir *redirs)
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

/* Helper 2: run the single builtin in parent; sets *status and returns 1 */
static int	parent_run_single_builtin(t_app *app, t_command *cmd,
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

/* Now ≤ 25 lines: checks, save/restore stdio, and delegate work */
static int	try_run_parent_builtin(t_app *app, t_command *cmd,
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


/* ---------------- child lifecycle and pipeline ---------------- */

/* Context to pass to child_exec to reduce parameters */
typedef struct s_child_io_ctx
{
	int	pipefd[2];
	int	io[4];
}	t_child_io_ctx;

/* Child code path: set signals, wire FDs, and exec command. */
static void	child_exec(t_app *app, t_command *cmd, t_env *env, t_child_io_ctx *c)
{
	int	infd;
	int	outfd;

	set_child_signals();
	infd = -1;
	outfd = -1;
	if (setup_redirections(app, *cmd->redirs, &infd, &outfd) < 0)
	{
		close_if_valid(&c->pipefd[0]);
		close_if_valid(&c->pipefd[1]);
		app_destroy(app);
		_exit(1);
	}
	child_apply_stdin(infd, c->io[0]);
	child_apply_stdout(outfd, c->io[3], c->pipefd[1]);
	close_if_valid(&c->pipefd[0]);
	close_if_valid(&c->pipefd[1]);
	close_if_valid(&infd);
	close_if_valid(&outfd);
	exec_command(app, cmd, env);
	_exit(126);
}

/* Prepare step: is_last flag and pipe creation */
static int	prepare_step(t_app *app, t_command *cmd, t_child_io_ctx *c)
{
	(void)app;
	c->io[1] = -1;
	c->io[2] = -1;
	c->io[3] = 0;
	if (!cmd->next)
		c->io[3] = 1;
	c->pipefd[0] = -1;
	c->pipefd[1] = -1;
	if (!c->io[3] && pipe(c->pipefd) < 0)
		return (-1);
	return (0);
}

/* Parent after fork: close write, close old tmp_in, carry read end. */
static void	parent_after_fork(int *tmp_in, t_child_io_ctx *c)
{
	close_if_valid(&c->io[1]);
	close_if_valid(&c->io[2]);
	close_if_valid(&c->pipefd[1]);
	close_if_valid(tmp_in);
	*tmp_in = c->pipefd[0];
}

/* Open/pipe, fork, child/parent split, return pid or -1. */
static pid_t	step_setup_and_fork(t_app *app, t_command *cmd,
									t_env *env, int *tmp_in)
{
	t_child_io_ctx	c;
	pid_t			pid;

	c.io[0] = *tmp_in;
	if (prepare_step(app, cmd, &c) < 0)
		return (-1);
	pid = fork();
	if (pid == 0)
		child_exec(app, cmd, env, &c);
	parent_after_fork(tmp_in, &c);
	return (pid);
}

/* Run all steps; return last status or 1 on early failure. */
static int	pipeline_run(t_app *app, t_env *env, t_command *head, pid_t *pids)
{
	int			i;
	int			tmp_in;
	t_command	*cur;
	pid_t		pid;

	i = 0;
	tmp_in = -1;
	cur = head;
	while (cur)
	{
		pid = step_setup_and_fork(app, cur, env, &tmp_in);
		if (pid < 0)
		{
			close_if_valid(&tmp_in);
			return (1);
		}
		pids[i] = pid;
		cur = cur->next;
		i += 1;
	}
	close_if_valid(&tmp_in);
	return (wait_pipeline(pids, i));
}

/* Public entry: parent-builtin fast path, else forked pipeline. */
int	execute_pipeline(t_app *app, t_command *head, t_env *env)
{
	int		n;
	int		code;

	if (!head)
		return (0);
	if (!head->next && head->argv && head->argv[0])
	{
		if (try_run_parent_builtin(app, head, env, &code))
			return (code);
	}
	n = count_commands(head);
	if (n <= 0)
		return (0);
	app->pids = (pid_t *)malloc(sizeof(pid_t) * n);
	if (!app->pids)
		return (1);
	code = pipeline_run(app, env, head, app->pids);
	free(app->pids);
	app->pids = NULL;
	return (code);
}
