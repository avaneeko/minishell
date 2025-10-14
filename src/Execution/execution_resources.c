/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_resources.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:59:28 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/14 16:44:42 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

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
