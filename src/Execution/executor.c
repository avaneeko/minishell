/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 22:22:36 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/14 17:21:35 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

/* ---------------- child lifecycle and pipeline ---------------- */
/* Child code path: set signals, wire FDs, and exec command. */
static void	child_exec(t_app *app, t_command *cmd, t_env *env,
	t_child_io_ctx *c)
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
