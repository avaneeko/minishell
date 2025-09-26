#include "minishell.h"
#include "execution_utils.h"
#include "Builtins/builtins_utils.h"
#include <unistd.h> /* close */
#include <stdio.h>  /* perror */

/* Count commands in a pipeline */
static int	count_commands(t_command *c)
{
	int	n;

	n = 0;
	while (c)
	{
		n++;
		c = c->next;
	}
	return (n);
}

/* Common early-exit cleanup path for this unit */
static int	abort_with(int n_cmd, int **pipes, pid_t *pids, const char *msg)
{
	perror(msg);
	close_and_free_pipes(n_cmd, pipes);
	free(pids);
	return (1);
}

/* Fork a child, wire pipes/redirs/signals, then exec */
static pid_t	fork_command(t_command *cmd, t_env *env, int **pipes, int n_cmd, int idx)
{
	pid_t	pid;
	int		i;

	pid = fork();
	if (pid == 0)
	{
		set_child_signals();
		set_pipe_ends(cmd, pipes, n_cmd, idx);
		i = 0;
		while (i < n_cmd - 1)
		{
			close(*pipes[i]);
			close(pipes[i][1]);
			i++;
		}
		set_redirs(cmd);
		exec_command(cmd, env);
	}
	return (pid);
}

int execute_pipeline(t_app * app, t_command *cmd, t_env *env)
{
    int         n_cmd;
    t_command   *cur;
    int         **pipes;
    pid_t       *pids;
    int         idx;

    n_cmd = count_commands(cmd);
    if (init_pipeline_resources(n_cmd, &pipes, &pids) == -1)
        return (1);
    cur = cmd;
    idx = 0;
    while (cur)
    {
        if (prepare_fds_for_command(app, cur) == -1)
            return (abort_with(n_cmd, pipes, pids, "minishell: redirection"));
        pids[idx] = fork_command(cur, env, pipes, n_cmd, idx);
        if (pids[idx] < 0)
            return (abort_with(n_cmd, pipes, pids, "minishell: fork"));
        cur = cur->next;
        idx++;
    }
    close_and_free_pipes(n_cmd, pipes);
    return (wait_pipeline(pids, n_cmd));
}
