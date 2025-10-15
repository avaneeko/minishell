/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:20:36 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 00:47:03 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Return a pointer to the value portion after KEY= or KEY+= in an export
 * 		argument.
 * @param arg Full argument containing a key then '=' or '+='.
 * @param key_len Length of the key portion.
 * @param is_append Non-zero if the operator is "+=" instead of "=".
 * @return Pointer to the first character of the value part in arg.
 */
static char const	*val_ptr_after_key(char const *arg, int key_len,
				int is_append)
{
	int	off;

	off = 1;
	if (is_append)
		off = 2;
	return (arg + key_len + off);
}

/**
 * @brief Apply export with either replace semantics (KEY=VAL) or append
 * 		semantics (KEY+=VAL).
 * @param env Environment to update.
 * @param key Key to modify.
 * @param val Value to assign or append.
 * @param is_append Non-zero to append to existing value, zero to replace.
 * @return 0 on success, 1 on failure to set/append.
 */
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

/**
 * @brief Handle an export argument that contains '=' or '+=' by splitting and
 * 		updating the environment.
 * @param env Environment to update.
 * @param arg Argument in the form KEY=VAL or KEY+=VAL.
 * @param key_len Length of KEY in arg.
 * @param is_append Non-zero if using "+=" append mode.
 * @return 0 on success, 1 on allocation or update failure.
 */
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

/**
 * @brief Process one export argument: validate identifier and dispatch to no-eq
 * 		or with-eq handler.
 * @param env Environment to update.
 * @param arg One export argument token.
 * @return 0 on success, 1 if identifier is invalid or update fails.
 */
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

/**
 * @brief Validate and apply export arguments: print sorted "declare -x" when
 * 		no args, or set/append variables.
 * @param argv Arguments with keys or key=value assignments.
 * @param env Environment to modify.
 * @return 0 on full success, 1 if any argument was invalid or failed to set.
 */
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
