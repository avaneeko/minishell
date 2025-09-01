
#include "minishell.h"
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/stat.h>

static int	is_executable_file(char *path)
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

void	free_string_array(char **strs)
{
	unsigned int i;

	i = 0;
	while (strs && strs[i])
	{
		free(strs[i]);
		i++;
	}
	free(strs);
}


/*
* Join two strings with '/' in between.
* Returns new malloc'ed string or NULL on failure.
*/
static char	*join_path(char const *dir, char const *file)
{
	int		len_dir;
	int		len_file;
	char	*res;
	int		i;

	len_dir = slen(dir);
	len_file = slen(file);
	res = malloc(len_dir + 1 + len_file + 1);
	if (!res)
		return (NULL);
	i = 0;
	while (i < len_dir)
	{
		res[i] = dir[i];
		i++;
	}
	res[i] = '/';
	i++;
	while (*file)
	{
		res[i] = *file;
		file++;
		i++;
	}
	res[i] = 0;
	return (res);
}

/*
* Search for command in paths specified by PATH env variable.
* Returns allocated string with full path if found or NULL.
*/
char	*find_command_path(char const *cmd, t_env const *env)
{
	t_epair		path_var;
	char		**paths;
	char		*full_path;
	unsigned int i;

	if (!cmd || !cmd[0])
		return (NULL);
	// If command contains '/', treat as direct path
	i = 0;
	while (cmd[i])
	{
		if (cmd[i] == '/')
		{
			if (is_executable_file((char *)cmd))
				return (strdup(cmd));
			else
				return (NULL);
		}
		i++;
	}
	// Lookup PATH environment variable
	if (!get_epair_by_key(env, "PATH", &path_var))
		return (NULL);
	// Split PATH by ':'
	paths = ft_split(path_var.value, ':'); // TODO: ADD ft_split
	if (!paths)
		return (NULL);
	i = 0;
	while (paths[i])
	{
		full_path = join_path(paths[i], cmd);
		if (!full_path)
		{
			free_string_array(paths);
			break;
		}
		if (is_executable_file(full_path))
		{
			free_string_array(paths);
			return (full_path);
		}
		free(full_path);
		i++;
	}
	free_string_array(paths);
	return (NULL);
}

/*TODO: ADD ft_split in utils*/