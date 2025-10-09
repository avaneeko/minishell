/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils2.c                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 20:03:33 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/09 20:03:52 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <sys/stat.h>                   /* stat, S_ISREG                 */
#include <unistd.h>                     /* access, X_OK                  */
#include <stdlib.h>                     /* free                          */

/* ************************************************************************** */
/*                                                                            */
/*                             execution_utils2.c                             */
/*                                                                            */
/*   Utility helpers: is_executable_file and array frees kept from EXECUTIONS.*/
/*                                                                            */
/* ************************************************************************** */
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
