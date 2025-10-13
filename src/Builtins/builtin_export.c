/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:20:36 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/13 22:22:46 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/* val_ptr_after_key: compute pointer to value after '=' or '+='.             */
static char const	*val_ptr_after_key(char const *arg, int key_len,
				int is_append)
{
	int	off;

	off = 1;
	if (is_append)
		off = 2;
	return (arg + key_len + off);
}

/* apply_set_or_append: call append or replace and map to 0/1 return.         */
static int	apply_set_or_append(t_env *env, char const *key,
		char const *val, int is_append)
{
	if (is_append)
	{
		if (!set_env_append(env, key, val))
			return (1);
		return (0);
	}
	if (!set_env_replace(env, key, val))
		return (1);
	return (0);
}

/* handle_export_with_eq: split, then set or append; now ≤25 lines.           */
static int	handle_export_with_eq(t_env *env, char const *arg,
		int key_len, int is_append)
{
	char		*key;
	char const	*val;
	int			ret;

	key = alloc_key_copy(arg, key_len);
	if (!key)
		return (1);
	val = val_ptr_after_key(arg, key_len, is_append);
	ret = apply_set_or_append(env, key, val, is_append);
	free(key);
	return (ret);
}

/* process_one_export_arg: validate and dispatch no-eq vs with-eq             */
static int	process_one_export_arg(t_env *env, char const *arg)
{
	int	key_len;
	int	is_append;
	int	has_eq;

	if (!parse_key_mode(arg, &key_len, &is_append, &has_eq))
	{
		print_export_error(arg);
		return (1);
	}
	if (!has_eq)
		return (handle_export_noeq(env, arg));
	return (handle_export_with_eq(env, arg, key_len, is_append));
}

int	builtin_export(char **argv, t_env *env)
{
	int	i;
	int	status;

	if (!argv || !argv[0] || !env)
		return (0);
	if (!argv[1])
	{
		if (print_export_noargs(env))
			return (0);
		return (1);
	}
	i = 1;
	status = 0;
	while (argv[i])
	{
		if (process_one_export_arg(env, argv[i]) != 0)
			status = 1;
		i += 1;
	}
	return (status);
}
