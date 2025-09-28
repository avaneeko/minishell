/* ************************************************************************** */
/*                                                                            */
/*                             execution_resources.c                           */
/*                                                                            */
/*   Legacy pipeline resource allocation helpers used by old matrix-pipes     */
/*   implementation; kept for compatibility and tests that still reference it.*/
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"                  /* basic types                   */
#include <unistd.h>                     /* pipe, close                   */
#include <stdlib.h>                     /* malloc, free                  */

/* Free partially created pipe pairs on failure.  */
static void	free_pipes_partial(int **pipes, int made)
{
	int	i;

	if (!pipes)
		return ;
	i = 0;
	while (i < made)
	{
		if (pipes[i])
		{
			close(pipes[i][0]);
			close(pipes[i][1]);
			free(pipes[i]);
		}
		i += 1;
	}
}

/* Allocate outer array for N-1 pipes if needed. */
static int	alloc_pipes_outer(int n_cmd, int ***pipes_ptr)
{
	int	**pipes;

	*pipes_ptr = NULL;
	if (n_cmd > 1)
	{
		pipes = (int **)malloc(sizeof(int *) * (n_cmd - 1));
		if (!pipes)
			return (-1);
		*pipes_ptr = pipes;
	}
	return (0);
}

/* Allocate pid array; free pipes on failure.  */
static int	alloc_pids_or_cleanup(int n_cmd, int **pipes, pid_t **pids_ptr)
{
	pid_t	*pids;

	pids = (pid_t *)malloc(sizeof(pid_t) * n_cmd);
	if (!pids)
	{
		if (pipes)
			free(pipes);
		return (-1);
	}
	*pids_ptr = pids;
	return (0);
}

/* Allocate and open all N-1 pipe pairs.  */
static int	alloc_open_pipes(int n_cmd, int ***pipes_ptr, pid_t **pids_ptr)
{
	int		i;
	int		**pipes;

	(void)pids_ptr;
	if (n_cmd <= 1)
		return (0);
	pipes = *pipes_ptr;
	i = 0;
	while (i < n_cmd - 1)
	{
		pipes[i] = (int *)malloc(sizeof(int) * 2);
		if (!pipes[i])
		{
			free_pipes_partial(pipes, i);
			free(pipes);
			*pipes_ptr = NULL;
			return (-1);
		}
		if (pipe(pipes[i]) < 0)
		{
			free_pipes_partial(pipes, i + 1);
			free(pipes);
			*pipes_ptr = NULL;
			return (-1);
		}
		i += 1;
	}
	return (0);
}

/* Public: allocate pids and legacy 2D pipes array.  */
int	init_pipeline_resources(int n_cmd, int ***pipes_ptr, pid_t **pids_ptr)
{
	if (alloc_pipes_outer(n_cmd, pipes_ptr) < 0)
		return (-1);
	if (alloc_pids_or_cleanup(n_cmd, *pipes_ptr, pids_ptr) < 0)
		return (-1);
	if (alloc_open_pipes(n_cmd, pipes_ptr, pids_ptr) < 0)
		return (-1);
	return (0);
}
