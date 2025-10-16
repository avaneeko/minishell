/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 20:03:33 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/15 23:31:32 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "execution_utils.h"

/**
 * @brief Return 1 if path refers to an executable regular file, else 0.
 * @param path Filesystem path to check.
 * @return 1 if stat says regular file and access says executable, 0 otherwise.
 */
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

/**
 * @brief Free a NULL-terminated array of strings allocated by ft_split or
 * 		env_serialize.
 * @param array Vector of strings to free; ignored if NULL.
 */
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

/**
 * @brief Close and free a partially created set of pipe pairs up to count
 * 		'made'.
 * @param pipes Outer array of int[2] pairs.
 * @param made Number of valid pairs to close and free.
 */
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

/**
 * @brief Allocate the outer array for (n_cmd - 1) pipe pairs if a pipeline is
 * 		needed.
 * @param n_cmd Number of pipeline commands.
 * @param pipes_ptr Output: on success, set to allocated outer array or NULL if
 * 		not needed.
 * @return 0 on success, -1 on allocation failure.
 */
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

/**
 * @brief Allocate the PIDs array; on failure, free the outer pipes array if it
 * 		was allocated.
 * @param n_cmd Number of commands to allocate PIDs for.
 * @param pipes Possibly allocated outer pipes array to free on failure.
 * @param pids_ptr Output: allocated pid_t array pointer on success.
 * @return 0 on success, -1 on allocation failure.
 */
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
