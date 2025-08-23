/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins.h                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 18:05:39 by jgueon            #+#    #+#             */
/*   Updated: 2025/08/23 19:24:33 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_H
# define BUILTINS_H

# include "minishell.h"

// Change the current working directory to argv[1] (or HOME if none)
int	builtin_cd(char **argv, t_env *env);

// Print its arguments to stdout, separated by spaces, ending with newline 
int	builtin_echo(char **argv);

// Print the current working directory to stdout
int	builtin_pwd(void);

// Print the environment to stdout (one KEY=VALUE pair per line)
int	builtin_env(t_env *env);

// Add or replace exported environment variables
int	builtin_export(char **argv, t_env *env);

// Unset (remove) environment variables by name
int	builtin_unset(char **argv, t_env *env);

// Exit the shell. Accepts optional numeric status argument.
int	builtin_exit(char **argv);

#endif
