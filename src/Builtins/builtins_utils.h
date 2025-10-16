/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.h                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:34:31 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/14 21:03:40 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef BUILTINS_UTILS_H
# define BUILTINS_UTILS_H

# include "minishell.h"
# include <string.h>
# include <linux/limits.h>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include "../Execution/execution_utils.h"
# include <linux/limits.h>

// Change the current working directory to argv[1] (or HOME if none)
int		builtin_cd(char **argv, t_env *env);

// Print its arguments to stdout, separated by spaces, ending with newline
int		builtin_echo(char **argv);

// Print the current working directory to stdout
int		builtin_pwd(char **argv, t_env *env);

// Print the environment to stdout (one KEY=VALUE pair per line)
int		builtin_env(t_env *env);

// Add or replace exported environment variables
int		builtin_export(char **argv, t_env *env);

// Unset (remove) environment variables by name
int		builtin_unset(char **argv, t_env *env);

// Exit the shell. Accepts optional numeric status argument.
int		builtin_exit_child(t_app *app, char **argv);
int		builtin_exit_parent(t_app *app, char **argv);

// env helpers
int		env_find_key(t_env *env, char *key);
int		env_set(t_env *env, char *key, char *value, int origin);
int		env_unset(t_env *env, char *key);

// lifbt function replica
char	*ft_strdup(const char *s1);

// builtin_execution
int		is_builtin(const char *cmd);
int		exec_builtin(t_app *app, char **argv, t_env *env);

/* Builtin helper functions */
void	print_str(int fd, const char *s);
void	print_export_error(char const *arg);
int		is_name_start(char c);
int		is_name_char(char c);
int		scan_key_body(char const *arg, int start);
int		parse_key_mode(char const *arg, int *key_len, int *is_append,
			int *has_eq);
size_t	strlen_or_zero(char const *s);
void	copy_bytes(char *dst, size_t *idx, char const *src);
char	*join_kv(char const *k, char const *v);
int		set_env_replace(t_env *env, char const *key, char const *val);
int		set_env_append(t_env *env, char const *key, char const *val);
int		exists_key(t_env *env, char const *key);
int		count_strv(char **v);
int		cmp_str(char const *a, char const *b);
void	swap_ptr(char **a, char **b);
int		bubble_one_pass(char **v, int n);
void	sort_lex(char **v, int n);
void	print_one_decl(char const *kv);
int		print_export_noargs(t_env const *env);
int		handle_export_noeq(t_env *env, char const *key);
char	*alloc_key_copy(char const *arg, int key_len);
int		print_line(int fd, char const *s);
char	*env_get_value(t_env *env, char const *key);
int		is_str_numeric(const char *s);
int		parse_sign(const char *s, int *idx, int *sign);
int		count_args(char **argv);
int		compute_limit_for_sign(int sign, unsigned long long *lim);
int		add_digit_check(unsigned long long *acc, unsigned long long lim, int d);
void	assign_signed_result(int sign, unsigned long long acc,
			unsigned long long lim, long long *out);
int		parse_digits_core(const char *s, int start, int sign, long long *out);
int		parse_ll(const char *s, long long *out);
int		to_status(long long v);
int		env_find_key(t_env *env, char *key);
int		env_unset(t_env *env, char *key);

#endif
