#include "minishell.h"
#include "./execution_utils.h"
// #include <sys/wait.h>
// #include <unistd.h>

/*
* Close unused pipe ends in parent/child.
* n_cmd: number of commands, pipes: array of n_cmd-1 pipefds.
* except: index to keep (use -1 if keeping none).
* is_write: 1 to keep only write, 0 for read.
*/
void	close_pipes_except(int **pipes, int n_cmd, int except, int is_write)
{
	int	idx;

	idx = 0;
	while (idx < n_cmd - 1)
	{
		if (!(idx == except && is_write))
			close(pipes[idx][1]);
		if (!(idx == except && !is_write))
			close(pipes[idx][0]);
		idx++;
	}
}

// // executor_helpers.c
// static void	set_pipe_ends(t_command *cmd, int **pipes, int n_cmd, int idx)
// {
// 	if (idx > 0)
// 		dup2(pipes[idx - 1], STDIN_FILENO);
// 	if (cmd->next)
// 		dup2(pipes[idx][1], STDOUT_FILENO);
// }

// static void	set_redirs(t_command *cmd)
// {
// 	if (cmd->infile != -1)
// 	{
// 		dup2(cmd->infile, STDIN_FILENO);
// 		close(cmd->infile);
// 	}
// 	if (cmd->outfile != -1)
// 	{
// 		dup2(cmd->outfile, STDOUT_FILENO);
// 		close(cmd->outfile);
// 	}
// }

// static void	exec_command(t_command *cmd, t_env *env)
// {
// 	if (cmd->is_builtin)
// 		exit(exec_builtin(cmd->argv, env));
// 	else
// 	{
// 		execvp(cmd->argv, cmd->argv);
// 		perror(cmd->argv);
// 		exit(127);
// 	}
// }

pid_t	fork_command(t_command *cmd, t_env *env, int **pipes, int n_cmd, int idx)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		set_pipe_ends(cmd, pipes, n_cmd, idx);
		close_pipes_except(pipes, n_cmd, idx, 0);
		set_redirs(cmd);
		exec_command(cmd, env);
	}
	return (pid);
}

/*
*	Helper function for "execute_pipeline"
*/
static void	init_pipeline_resources(int n_cmd, int ***pipes_ptr, pid_t **pids_ptr)
{
	int	**pipes;
	pid_t	*pids;
	int	i;

	pipes = malloc(sizeof(int *) * (n_cmd - 1));
	pids = malloc(sizeof(pid_t) * n_cmd);
	i = 0;
	while (i < n_cmd - 1)
	{
		pipes[i] = malloc(sizeof(int) * 2);
		pipe(pipes[i]);
		i++;
	}
	*pipes_ptr = pipes;
	*pids_ptr = pids;
}

// static void	close_and_free_pipes(int n_cmd, int **pipes)
// {
// 	int	i;

// 	i = 0;
// 	while (i < n_cmd - 1)
// 	{
// 		close(pipes[i]);
// 		close(pipes[i][1]);
// 		free(pipes[i]);
// 		i++;
// 	}
// 	free(pipes);
// }

// static int	wait_pipeline(pid_t *pids, int n_cmd)
// {
// 	int	i;
// 	int	status;
// 	int	exit_code = 1;

// 	i = 0;
// 	while (i < n_cmd)
// 	{
// 		waitpid(pids[i], &status, 0);
// 		if (WIFEXITED(status))
// 			exit_code = WEXITSTATUS(status);
// 		i++;
// 	}
// 	free(pids);
// 	return (exit_code);
// }

int	execute_pipeline(t_command *cmd, t_env *env)
{
	int		n_cmd;
	t_command	*cur;
	int		**pipes;
	pid_t		*pids;
	int		idx;

	n_cmd = 0;
	cur = cmd;
	while (cur)
	{
		n_cmd++;
		cur = cur->next;
	}
	init_pipeline_resources(n_cmd, &pipes, &pids);
	cur = cmd;
	idx = 0;
	while (cur)
	{
		pids[idx] = fork_command(cur, env, pipes, n_cmd, idx);
		cur = cur->next;
		idx++;
	}
	close_and_free_pipes(n_cmd, pipes);
	return (wait_pipeline(pids, n_cmd));
}

