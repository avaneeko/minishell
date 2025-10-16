/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_resources.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:59:28 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 15:53:07 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

/**
 * @brief Allocate an int[2] pipe slot at index i inside the outer pipes array.
 * @param i Index into the outer array for the pipe pair.
 * @param pipes Outer array of int* where each entry points to a 2-int pipe.
 * @return 0 on success, -1 on malloc failure.
 */
static int	alloc_one_pipe_pair(int i, int **pipes)
{
	pipes[i] = (int *)malloc(sizeof(int) * 2);
	if (!pipes[i])
		return (-1);
	return (0);
}

/**
 * @brief Open both ends of the pipe at index i for a previously allocated pair.
 * @param i Index into the outer pipes array.
 * @param pipes Outer array of int[2] pointers.
 * @return 0 on success, -1 if pipe() fails.
 */
static int	open_one_pipe_pair(int i, int **pipes)
{
	if (pipe(pipes[i]) < 0)
		return (-1);
	return (0);
}

/**
 * @brief Create and open all (n_cmd - 1) pipe pairs; if any step fails, close
 * 		and free what was made.
 * @param n_cmd Number of commands in the pipeline.
 * @param pipes Outer array where each entry is an int[2] pair.
 * @return 0 on success, -1 on allocation or pipe failure.
 */
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

/**
 * @brief Ensure that all needed pipe pairs are opened or perform full cleanup
 * 		on failure.
 * @param n_cmd Number of commands in pipeline.
 * @param pipes_ptr Pointer to the outer pipes array pointer for possible
 * 		 NULLing.
 * @param pids_ptr Unused here; present for symmetry with other allocators.
 * @return 0 on success, -1 on failure with cleanup.
 */
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

/**
 * @brief Public entry to allocate PIDs and the legacy 2D pipes matrix for
 * 		a pipeline.
 * @param n_cmd Number of commands in the pipeline.
 * @param pipes_ptr Output: allocated outer array of pipe pairs if needed.
 * @param pids_ptr Output: allocated array of PIDs sized for n_cmd.
 * @return 0 on success, -1 on any allocation or opening failure.
 */
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
