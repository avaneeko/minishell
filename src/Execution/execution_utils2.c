#include "./execution_utils.h"
#include "minishell.h"

void	ft_split_free(char **array)
{
	int i = 0;
	if (!array)
		return;
	while (array[i])
		free(array[i++]);
	free(array);
}

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

// executor_helpers.c(for fork_command)
static void	set_pipe_ends(t_command *cmd, int **pipes, int n_cmd, int idx)
{
	if (idx > 0)
		dup2(pipes[idx - 1], STDIN_FILENO);
	if (cmd->next)
		dup2(pipes[idx][1], STDOUT_FILENO);
}

// executor_helpers.c(for fork_command)
static void	set_redirs(t_command *cmd)
{
	if (cmd->infile != -1)
	{
		dup2(cmd->infile, STDIN_FILENO);
		close(cmd->infile);
	}
	if (cmd->outfile != -1)
	{
		dup2(cmd->outfile, STDOUT_FILENO);
		close(cmd->outfile);
	}
}
