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
// (close both ends and free pair)
// This is used on any partial failure during allocation/open.
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

/* ************************************************************************** */
/* Allocate the outer array of (n_cmd - 1) int[2] pointers when needed.       */
/* Returns 0 on success, -1 on malloc failure.             					  */
/* ************************************************************************** */
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

/* ************************************************************************** */
/* Allocate PIDs array; if it fails, free the outer pipes array (if any).     */
/* Returns 0 on success, -1 on failure. 				                      */
/* ************************************************************************** */
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

/* ************************************************************************** */
/* Allocate one int[2] pair at index i of the pipes outer array.              */
/* Returns 0 on success, -1 on malloc failure. 					              */
/* ************************************************************************** */
static int	alloc_one_pipe_pair(int i, int **pipes)
{
	pipes[i] = (int *)malloc(sizeof(int) * 2);
	if (!pipes[i])
		return (-1);
	return (0);
}

/* ************************************************************************** */
/* Open the pipe for the allocated pair at index i (both ends).               */
/* Returns 0 on success, -1 on pipe() failure. 					              */
/* ************************************************************************** */
static int	open_one_pipe_pair(int i, int **pipes)
{
	if (pipe(pipes[i]) < 0)
		return (-1);
	return (0);
}

/* ************************************************************************** */
/* Create and open all (n_cmd - 1) pipe pairs; on failure, cleanup partial.   */
/* Returns 0 on success, -1 on alloc/pipe failure. 					          */
/* ************************************************************************** */
static int	open_pipes_loop(int n_cmd, int **pipes)
{
	int	i;

	if (n_cmd <= 1 || !pipes)
		return (0);
	i = 0;
	while (i < n_cmd - 1)
	{
		if (alloc_one_pipe_pair(i, pipes) < 0)
		{
			free_pipes_partial(pipes, i);
			return (-1);
		}
		if (open_one_pipe_pair(i, pipes) < 0)
		{
			free_pipes_partial(pipes, i + 1);
			return (-1);
		}
		i += 1;
	}
	return (0);
}

/* ************************************************************************** */
/* Wrapper: assume outer array is allocated; open all pairs or fully cleanup. */
/* On failure, free the outer array too and nullify the pointer. 			  */
/* ************************************************************************** */
static int	alloc_open_pipes(int n_cmd, int ***pipes_ptr, pid_t **pids_ptr)
{
	int	**pipes;

	(void)pids_ptr;
	pipes = *pipes_ptr;
	if (n_cmd <= 1 || !pipes)
		return (0);
	if (open_pipes_loop(n_cmd, pipes) < 0)
	{
		free(pipes);
		*pipes_ptr = NULL;
		return (-1);
	}
	return (0);
}

/* ************************************************************************** */
/* Public entry: allocate pids and the legacy 2D pipes matrix (N-1 pairs).    */
/* Returns 0 on success, -1 on failure (with proper cleanup). 				  */
/* ************************************************************************** */
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
