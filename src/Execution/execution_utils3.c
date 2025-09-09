#include "./execution_utils.h"
#include "minishell.h"

/*
*	// executor_helpers.c(for fork_command)
*/
static void	exec_command(t_command *cmd, t_env *env)
{
	if (cmd->is_builtin)
		exit(exec_builtin(cmd->argv, env));
	else
	{
		execvp(cmd->argv, cmd->argv);
		perror(cmd->argv);
		exit(127);
	}
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

/*
*	Helper function for "execute_pipeline"
*/
static void	close_and_free_pipes(int n_cmd, int **pipes)
{
	int	i;

	i = 0;
	while (i < n_cmd - 1)
	{
		close(pipes[i]);
		close(pipes[i][1]);
		free(pipes[i]);
		i++;
	}
	free(pipes);
}

/*
*	Helper function for "execute_pipeline"
*/
static int	wait_pipeline(pid_t *pids, int n_cmd)
{
	int	i;
	int	status;
	int	exit_code = 1;

	i = 0;
	while (i < n_cmd)
	{
		waitpid(pids[i], &status, 0);
		if (WIFEXITED(status))
			exit_code = WEXITSTATUS(status);
		i++;
	}
	free(pids);
	return (exit_code);
}

/**
 * @brief Compares two strings up to n characters
 *
 * This function compares the two strings s1 and s2 lexicographically,
 * but at most n characters are compared. It returns an integer less than,
 * equal to, or greater than zero if s1 is found, respectively, 
 * to be less than, equal to, or greater than s2.
 *
 * @param s1: The first string to compare
 * @param s2: The second string to compare
 * @param n: The maximum number of characters to compare
 *
 * @return An integer less than, equal to, or greater than zero if s1 is found,
 *          respectively, to be less than, equal to, or greater than s2
 */
int	ft_strncmp(const char *s1, const char *s2, size_t n)
{
	size_t	i;

	i = 0;
	if (n == 0)
		return (0);
	while (s1[i] && s2[i] && i < n - 1 && s1[i] == s2[i])
		i++;
	return ((unsigned char)s1[i] - ((unsigned char)s2[i]));
}
