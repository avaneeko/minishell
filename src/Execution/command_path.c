/* ****************************************************************************/
/*                                                                            */
/*                               commandpath.c                                */
/*                                                                            */
/*   PATH resolution compatible with EXECUTIONS.c helpers: uses PATH from env,*/
/*   ft_split on ':', then checks each candidate with is_executable_file.     */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"                  /* t_env, t_epair, get_epair_by_key */
#include <stdlib.h>                     /* malloc, free                      */
#include "execution_utils.h"

/* Join dir and file with '/', allocating a new string. [attached_file:1] */
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

/* If cmd has slashes, check it directly. */
static char	*find_direct_path(char const *cmd)
{
	if (is_executable_file(cmd))
		return ((char *)mclone(cmd, slen(cmd) + 1));
	return (NULL);
}

/* Search PATH entries for an executable command.  */
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

/* Resolve a command to an absolute path using PATH, or return NULL. */
char	*find_command_path(char const *cmd, t_env const *env)
{
	t_epair	pathvar;
	char	**paths;
	unsigned int i;

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
