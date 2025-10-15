/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils3.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 20:04:08 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/15 23:34:37 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

/**
 * @brief Close fd if non-negative and set it to -1 to avoid reuse.
 * @param fd Pointer to file descriptor to close and invalidate.
 */
void	close_if_valid(int *fd)
{
	if (fd && *fd >= 0)
	{
		close(*fd);
		*fd = -1;
	}
}

/**
 * @brief Close both ends of a pipe pair if the array is not NULL.
 * @param pipefd An array of two file descriptors.
 */
void	close_pipe_pair(int pipefd[2])
{
	if (pipefd)
	{
		close_if_valid(&pipefd[0]);
		close_if_valid(&pipefd[1]);
	}
}

/**
 * @brief Free a serialized environment array created by env_serialize.
 * @param envp NULL-terminated "KEY=VALUE" array to free.
 */
void	env_free_serialized(char **envp)
{
	unsigned int	i;

	if (!envp)
		return ;
	i = 0;
	while (envp[i])
	{
		free(envp[i]);
		i += 1;
	}
	free(envp);
}

/**
 * @brief Close all N-1 pipe pairs created for a pipeline and free the outer
 * 		array.
 * @param n_cmd Number of commands, determining how many pipe pairs exist.
 * @param pipes Outer array of int[2] pipe pairs.
 */
void	close_and_free_pipes(int n_cmd, int **pipes)
{
	int	i;

	if (!pipes)
		return ;
	i = 0;
	while (i < n_cmd - 1)
	{
		if (pipes[i])
		{
			close(pipes[i][0]);
			close(pipes[i][1]);
			free(pipes[i]);
		}
		i += 1;
	}
	free(pipes);
}

/**
 * @brief Count how many t_command nodes are linked starting from head.
 * @param head First node of the command list.
 * @return Number of commands in the list.
 */
int	count_commands(t_command *head)
{
	int	n;

	n = 0;
	while (head)
	{
		n += 1;
		head = head->next;
	}
	return (n);
}
