/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   command_path.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:47:11 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/15 21:40:15 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

/**
 * @brief Join a directory and filename with a single '/' between them,
 *			allocating a new string.
 * @param dir Directory path without trailing slash, not NULL.
 * @param file Filename or subpath to append, not NULL.
 * @return Newly allocated "dir/file" string on success, or NULL on
 * 			allocation failure.
 */
static char	*join_path(char const *dir, char const *file)
{
	size_t	ld;
	size_t	lf;
	char	*r;
	size_t	i;

	ld = slen(dir);
	lf = slen(file);
	r = (char *)malloc(ld + 1 + lf + 1);
	if (!r)
		return (NULL);
	i = 0;
	while (i < ld)
	{
		r[i] = dir[i];
		i += 1;
	}
	r[ld] = '/';
	i = 0;
	while (i < lf)
	{
		r[ld + 1 + i] = file[i];
		i += 1;
	}
	r[ld + 1 + lf] = '\0';
	return (r);
}

/**
 * @brief If the command contains a slash, check the path directly and return a
 * 		copy if executable.
 * @param cmd Command string that may include '/' characters.
 * @return Newly allocated copy of cmd if it is an executable regular file,
 * 		otherwise NULL.
 */
static char	*find_direct_path(char const *cmd)
{
	if (is_executable_file(cmd))
		return ((char *)mclone(cmd, slen(cmd) + 1));
	return (NULL);
}

/**
 * @brief Search an array of PATH entries for an executable named cmd.
 * @param paths NULL-terminated array of directory strings to search.
 * @param cmd Command name without slashes to look for.
 * @return Newly allocated absolute path to the executable if found, or NULL if
 * 		not found or on allocation failure.
 */
static char	*search_in_paths(char **paths, char const *cmd)
{
	unsigned int	i;
	char			*full;

	i = 0;
	while (paths && paths[i])
	{
		full = join_path(paths[i], cmd);
		if (!full)
			break ;
		if (is_executable_file(full))
		{
			ft_split_free(paths);
			return (full);
		}
		free(full);
		i += 1;
	}
	ft_split_free(paths);
	return (NULL);
}

/**
 * @brief Resolve a command name to an absolute path using PATH or return NULL
 * 		if not resolvable.
 * @param cmd Command to resolve; if it contains '/', it is checked directly.
 * @param env Environment list to read PATH from.
 * @return Newly allocated absolute path, or NULL if not found or on errors.
 */
char	*find_command_path(char const *cmd, t_env const *env)
{
	t_epair			pathvar;
	char			**paths;
	unsigned int	i;

	if (!cmd || !cmd[0])
		return (NULL);
	i = 0;
	while (cmd[i])
	{
		if (cmd[i] == '/')
			return (find_direct_path(cmd));
		i += 1;
	}
	if (!get_epair_by_key(env, "PATH", &pathvar))
		return (NULL);
	paths = ft_split(pathvar.value, ':');
	if (!paths)
		return (NULL);
	return (search_in_paths(paths, cmd));
}
