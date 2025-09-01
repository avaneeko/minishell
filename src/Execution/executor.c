#include "minishell.h"
#include <sys/wait.h>
#include <unistd.h>

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

/*
* Function to fork-execute one command in the pipeline.
* index: index in pipeline.
*/
pid_t	fork_command(t_command *cmd, t_env *env, int **pipes, int n_cmd, int idx)
{
	pid_t	pid;

	pid = fork();
	if (pid == 0)
	{
		if (idx > 0)
		{
			dup2(pipes[idx - 1][0], STDIN_FILENO);
		}
		if (cmd->next != NULL)
		{
			dup2(pipes[idx][1], STDOUT_FILENO);
		}
		close_pipes_except(pipes, n_cmd, idx, 0);
		if (cmd->infile != -1)
		{
			dup2(cmd->infile, STDIN_FILENO);
			close(cmd->infile);
		}
		if (cmd->outfile != -1)
		{
			dup2(cmd->outfile, STDOUT_FILENO);
			close(cmd->outfile);
		}
		if (cmd->is_builtin)
		{
			exit(exec_builtin(cmd->argv, env));
		}
		else
		{
			execvp(cmd->argv[0], cmd->argv);
			perror(cmd->argv[0]);
			exit(127);
		}
	}
	return (pid);
}

/*
* Execute a pipeline of t_command linked list.
* Returns last exit code.
*/
int	execute_pipeline(t_command *cmd, t_env *env)
{
	int		n_cmd;
	t_command	*cur;
	int		**pipes;
	pid_t	*pids;
	int		idx;
	int		status;
	int		exit_code;

	n_cmd = 0;
	cur = cmd;
	while (cur)
	{
		n_cmd++;
		cur = cur->next;
	}
	pipes = malloc(sizeof(int *) * (n_cmd - 1));
	pids = malloc(sizeof(pid_t) * n_cmd);
	idx = 0;
	while (idx < n_cmd - 1)
	{
		pipes[idx] = malloc(sizeof(int) * 2);
		pipe(pipes[idx]);
		idx++;
	}
	cur = cmd;
	idx = 0;
	while (cur)
	{
		pids[idx] = fork_command(cur, env, pipes, n_cmd, idx);
		cur = cur->next;
		idx++;
	}
	idx = 0;
	while (idx < n_cmd - 1)
	{
		close(pipes[idx][0]);
		close(pipes[idx][1]);
		free(pipes[idx]);
		idx++;
	}
	free(pipes);
	idx = 0;
	while (idx < n_cmd)
	{
		waitpid(pids[idx], &status, 0);
		if (WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		else
			exit_code = 1;
		idx++;
	}
	free(pids);
	return (exit_code);
}
