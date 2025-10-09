/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.h                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 20:01:14 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/09 20:03:24 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef EXECUTION_UTILS_H
# define EXECUTION_UTILS_H

# include "minishell.h"

/* libft-like */
char	**ft_split(const char *s, char c);
void	ft_split_free(char **array);
void	free_string_array(char **strs);

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

/* path utils */
int		is_executable_file(char const *path);

/* redirection setup  */
int		setup_redirections(t_app *app, t_redir *redirs, int *infd, int *outfd);
int		handle_input_redirection(t_app const *app, t_redir redir, int *infd);

/* pipes */
int		wait_pipeline(pid_t *pids, int n_cmd);

/* FD helpers */
void	close_if_valid(int *fd);
void	close_pipe_pair(int pipefd[2]);

#endif
