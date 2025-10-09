/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils3.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 20:04:08 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/09 20:04:44 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <unistd.h>                     /* close  */
#include <stdlib.h>                     /* free   */

/* ************************************************************************** */
/*                                                                            */
/*                             executionutils3.c                              */
/*                                                                            */
/*   Small cross-file utilities: FD closers, env_free_serialized, and legacy  */
/*   close_and_free_pipes for the old pipe-matrix implementation.             */
/*                                                                            */
/* ************************************************************************** */
/* Close fd if valid and set to -1. */
void	close_if_valid(int *fd)
{
	if (fd && *fd >= 0)
	{
		close(*fd);
		*fd = -1;
	}
}

/* Close both ends of a 2-int pipe pair if valid.  */
void	close_pipe_pair(int pipefd[2])
{
	if (pipefd)
	{
		close_if_valid(&pipefd[0]);
		close_if_valid(&pipefd[1]);
	}
}

/* Free a NULL-terminated env array created by env_serialize.  */
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

/* Legacy: close N-1 pipes and free the outer array.  */
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
