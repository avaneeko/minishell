/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 20:01:14 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 15:32:24 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTION_UTILS_H
# define EXECUTION_UTILS_H

# include "minishell.h"
# include "../Builtins/builtins_utils.h"
# include <unistd.h>	/* close/write/execve */
# include <stdlib.h>	/* exit/ free */
# include <sys/stat.h>	/* stat, S_IREG */
# include <errno.h>		/* O_ / EINTR */
# include <string.h>	/*strerror */
# include <fcntl.h>		/*open flags */
# include <stdint.h>	/* uintptr_t */
# include <sys/wait.h>	/* waitpid/WIF* macros */
# include <signal.h>	/* SIGINT/ SIGQUIT */

typedef struct s_app		t_app;
typedef struct s_env		t_env;
typedef struct s_command	t_command;
typedef struct s_redir		t_redir;

/* Context to pass to child_exec to reduce parameters */
typedef struct s_child_io_ctx
{
	int	pipefd[2];
	int	io[4];
}	t_child_io_ctx;

/* libft-like */
char	**ft_split(const char *s, char c);
void	ft_split_free(char **array);

/* env serialization */
char	**env_serialize(const t_env *env);
void	env_free_serialized(char **envp);

/* path resolution */
char	*find_command_path(const char *cmd, const t_env *env);

/* executor helpers */
void	set_pipe_ends(t_command *cmd, int **pipes, int n_cmd, int idx);
void	set_redirs(t_command *cmd);
int		init_pipeline_resources(int n_cmd, int ***pipes_ptr, pid_t **pids_ptr);
void	close_and_free_pipes(int n_cmd, int **pipes);
int		wait_pipeline(pid_t *pids, int n_cmd);
void	exec_command(t_app *app, t_command *cmd, t_env *env);
int		execute_pipeline(t_app *app, t_command *cmd, t_env *env);
int		count_commands(t_command *head);
void	save_stdio(int saved[2]);
void	restore_stdio(int saved[2]);
void	child_apply_stdin(int infile, int tmp_in);
void	child_apply_stdout(int outfile, int is_last, int pipe_w);
int		is_parent_builtin(char const *name);
void	parent_apply_redirs(int infd, int outfd);
int		parent_open_and_apply_redirs(t_app *app, t_redir *redirs);
int		parent_run_single_builtin(t_app *app, t_command *cmd,
			t_env *env, int *status);
int		try_run_parent_builtin(t_app *app, t_command *cmd,
			t_env *env, int *status);
void	parent_after_fork(int *tmp_in, t_child_io_ctx *c);

/* path utils */
int		is_executable_file(char const *path);

/* redirection setup  */
int		setup_redirections(t_app *app, t_redir *redirs, int *infd, int *outfd);
int		handle_input_redirection(t_app const *app, t_redir redir, int *infd);

/* pipes and execution resources helpers */
int		wait_pipeline(pid_t *pids, int n_cmd);
void	free_pipes_partial(int **pipes, int made);
int		alloc_pipes_outer(int n_cmd, int ***pipes_ptr);
int		alloc_pids_or_cleanup(int n_cmd, int **pipes, pid_t **pids_ptr);

/* FD helpers */
void	close_if_valid(int *fd);
void	close_pipe_pair(int pipefd[2]);

#endif
