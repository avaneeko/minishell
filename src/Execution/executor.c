/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 22:22:36 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 15:32:58 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

// From signals.c
void	set_parent_signals(void);

/**
 * @brief Child path: set signals, open redirections, wire stdio, then exec the
 * 		command.
 * @param app Application context used for cleanup before exiting the child.
 * @param cmd Command node holding argv and the redirection list.
 * @param env Environment variables for builtins or external execution.
 * @param c Child I/O context carrying previous read end and current pipe ends.
 * @details On redirection setup failure, closes pipe ends, destroys app,
 * 		and _exit(1).
 * @post Does not return on success; falls back to _exit(126) if exec cannot
 * 		be performed.
 */
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

/**
 * @brief Initialize step context, detect last command, and create a pipe if not
 * 		last.
 * @param app Unused in this helper.
 * @param cmd Current command; absence of cmd->next marks this as the last step.
 * @param c I/O context to initialize; sets is_last flag in c->io[3] and
 * 		initializes pipefd[].
 * @return 0 on success, -1 if pipe() fails when a pipe is required.
 */
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

/**
 * @brief Prepare descriptors, fork the child to run the step, and handle
 * 		parent-side FD handoff.
 * @param app Application context.
 * @param cmd Command node to execute in this step.
 * @param env Environment passed to the child.
 * @param tmp_in In/out: previous pipe read end carried into the next step by
 * 		the parent.
 * @return PID in the parent, -1 on setup failure before fork; the child does
 * 		not return.
 */
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

/**
 * @brief Iterate the pipeline: fork each step, record PIDs, then wait and
 * 		return the final status.
 * @param app Application context.
 * @param env Environment for child execution.
 * @param head Head of the command list to execute.
 * @param pids Output array where each created child PID is stored in order.
 * @return Final status from wait_pipeline, or 1 on early setup/fork failure.
 */
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

/**
 * @brief Execute a pipeline: run parent-fast-path builtins when possible,
 * 		else fork children and wait.
 * @param app Application context (stores pids and last exit code).
 * @param head Head of the command list (pipeline).
 * @param env Environment variables.
 * @return Final pipeline status code.
 */
int	execute_pipeline(t_app *app, t_command *head, t_env *env)
{
	int		n;
	int		code;

	set_parent_signals();
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
