/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils5.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 22:21:12 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 21:14:12 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Bubble-sort a string vector lexicographically, looping until no swaps
 * 		occur.
 * @param v Vector of strings to sort.
 * @param n Count of elements.
 */
void	sort_lex(char **v, int n)
{
	int	changed;

	changed = 1;
	while (changed)
	{
		changed = bubble_one_pass(v, n);
	}
}

/**
 * @brief Print a serialized env entry as: declare -x KEY="VALUE".
 * @param kv A "KEY=VALUE" string, where VALUE may be empty.
 */
void	print_one_decl(char const *kv)
{
	int	i;

	print_str(1, "declare -x ");
	i = 0;
	while (kv[i] && kv[i] != '=')
	{
		write(1, &kv[i], 1);
		i += 1;
	}
	if (kv[i] == '=')
	{
		print_str(1, "=\"");
		i += 1;
		while (kv[i])
		{
			write(1, &kv[i], 1);
			i += 1;
		}
		print_str(1, "\"");
	}
	print_str(1, "\n");
}

/**
 * @brief Ensure a key exists for export without '=', creating it with empty
 * 		value if missing.
 * @param env Environment to modify.
 * @param key Key name to ensure.
 * @return 0 on success, 1 on failure to create.
 */
int	handle_export_noeq(t_env *env, char const *key)
{
	if (!exists_key(env, key))
	{
		if (!set_env_replace(env, key, ""))
			return (1);
	}
	return (0);
}

/**
 * @brief Allocate and copy a key substring of length key_len from arg.
 * @param arg Full argument containing the key prefix.
 * @param key_len Number of characters to copy.
 * @return Newly allocated key string or NULL on failure.
 */
char	*alloc_key_copy(char const *arg, int key_len)
{
	char	*key;
	int		i;

	key = (char *)malloc((size_t)key_len + 1);
	if (!key)
		return (0);
	i = 0;
	while (i < key_len)
	{
		key[i] = arg[i];
		i += 1;
	}
	key[key_len] = 0;
	return (key);
}
