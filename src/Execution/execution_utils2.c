/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 20:03:33 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/14 16:46:24 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execution_utils.h"

/* Check that path is a regular file and is executable.  */
int	is_executable_file(char const *path)
{
	struct stat	st;

	if (stat(path, &st) != 0)
		return (0);
	if (!S_ISREG(st.st_mode))
		return (0);
	if (access(path, X_OK) != 0)
		return (0);
	return (1);
}

/* Free a NULL-terminated string array. */
void	ft_split_free(char **array)
{
	unsigned int	i;

	if (!array)
		return ;
	i = 0;
	while (array[i])
	{
		free(array[i]);
		i += 1;
	}
	free(array);
}

/* Free partially created pipe pairs on failure.  */
// (close both ends and free pair)
// This is used on any partial failure during allocation/open.
void	free_pipes_partial(int **pipes, int made)
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
int	alloc_pipes_outer(int n_cmd, int ***pipes_ptr)
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
int	alloc_pids_or_cleanup(int n_cmd, int **pipes, pid_t **pids_ptr)
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
