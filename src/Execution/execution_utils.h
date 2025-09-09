#ifndef EXECUTION_UTILS_H
# define EXECUTION_UTILS_H

# include "minishell.h"
// # include <stdlib.h>
# include <string.h>
// # include <unistd.h>
# include <sys/stat.h>
# include <errno.h>
# include <fcntl.h>
# include <stdio.h>
# include <sys/types.h>
// # include <signal.h>
// # include <sys/wait.h>

//libft function replicas
char	**ft_split(const char *s, char c);
size_t	ft_strlen(const char *s);
int		ft_strncmp(const char *s1, const char *s2, size_t n);

// Frees a NULL-terminated char** returned by ft_split.
void	ft_split_free(char **array);

static int is_executable_file(char *path);

void	free_string_array(char **strs);

// executor_helpers.c (for fork_command)
static void	set_pipe_ends(t_command *cmd, int **pipes, int n_cmd, int idx);
static void	set_redirs(t_command *cmd);
// helpers for execute_pipeline function
static void	init_pipeline_resources(int n_cmd, int ***pipes_ptr, pid_t **pids_ptr);
static void	close_and_free_pipes(int n_cmd, int **pipes);
static int	wait_pipeline(pid_t *pids, int n_cmd);




#endif