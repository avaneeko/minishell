
#include "minishell.h"
#include "./execution_utils.h"
// #include <stdlib.h>
// #include <string.h>
// #include <unistd.h>
// #include <sys/stat.h>

// static int	is_executable_file(char *path)
// {
// 	struct stat	st;

// 	if (stat(path, &st) != 0)
// 		return (0);
// 	if (!S_ISREG(st.st_mode))
// 		return (0);
// 	if (access(path, X_OK) != 0)
// 		return (0);
// 	return (1);
// }

// void	free_string_array(char **strs)
// {
// 	unsigned int i;

// 	i = 0;
// 	while (strs && strs[i])
// 	{
// 		free(strs[i]);
// 		i++;
// 	}
// 	free(strs);
// }

/*
* Helper function for *join_path
*
*/
static void write_join(char *res, const char *dir, const char *file, int len_dir)
{
    int i;

	i = 0;
    while (i < len_dir)
        res[i] = dir[i++];
    res[i++] = '/';
    while (*file)
        res[i++] = *file++;
    res[i] = 0;
}

/*
* Join two strings with '/' in between.
* Returns new malloc'ed string or NULL on failure.
*/
static char *join_path(const char *dir, const char *file)
{
    int len_dir;
    int len_file;
    char *res;

	len_dir = slen(dir);
	len_file = slen(file);
	res = malloc(len_dir + 1 + len_file + 1);
    if (!res)
        return (NULL);
    write_join(res, dir, file, len_dir);
    return res;
}

/*
* Helper function for *find_command_path
*
*/
static char *find_direct_path(const char *cmd)
{
    if (is_executable_file((char *)cmd))
        return (strdup(cmd));
    return (NULL);
}

/*
*	Helper function for *find_command_path
*/
static char *search_in_paths(char **paths, const char *cmd)
{
    unsigned int i;
    char *full_path;

	i = 0;
    while (paths[i])
	{
        full_path = join_path(paths[i], cmd);
        if (!full_path)
            break;
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

char *find_command_path(const char *cmd, const t_env *env)
{
    t_epair path_var;
    char **paths;
    unsigned int i = 0;
    if (!cmd || !cmd)
        return NULL;
    while (cmd[i])
        if (cmd[i++] == '/')
            return find_direct_path(cmd);
    if (!get_epair_by_key(env, "PATH", &path_var))
        return NULL;
    paths = ft_split(path_var.value, ':');
    if (!paths)
        return NULL;
    return search_in_paths(paths, cmd);
}
