/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env_helpers.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:37:04 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 00:50:40 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Update an existing environment entry at index with a new value and
 * 		origin.
 * @param env Environment container.
 * @param idx Index of the existing pair to modify.
 * @param value New value; NULL is stored as an empty string.
 * @param origin Origin metadata to set on the pair.
 * @return 1 on success, 0 on allocation failure or invalid index.
 */
static int	update_existing(t_env *env, int idx, char *value, int origin)
{
	char	*dup;

	if (!env || idx < 0 || idx >= (int)env->len)
		return (0);
	if (value == NULL)
		dup = ft_strdup("");
	else
		dup = ft_strdup(value);
	if (!dup)
		return (0);
	free(env->pairs[idx].value);
	env->pairs[idx].value = dup;
	env->pairs[idx].origin = origin;
	return (1);
}

/**
 * @brief Initialize a t_epair with heap-duplicated key and value plus origin.
 * @param pair Output pair to initialize.
 * @param key Key string to duplicate; must not be NULL.
 * @param value Value to duplicate; NULL becomes an empty string.
 * @param origin Origin metadata to assign.
 * @return 1 on success, 0 on allocation failure or invalid input.
 */
static int	init_pair_values(t_epair *pair, char *key, char *value, int origin)
{
	if (!pair || !key)
		return (0);
	pair->key = ft_strdup(key);
	if (value)
		pair->value = ft_strdup(value);
	else
		pair->value = ft_strdup("");
	pair->origin = origin;
	if (!pair->key || !pair->value)
		return (0);
	return (1);
}

/**
 * @brief Free key and value inside a t_epair if present; does not free the
 * 		pair itself.
 * @param pair Pair whose fields will be freed.
 */
static void	free_pair_values(t_epair *pair)
{
	if (!pair)
		return ;
	if (pair->key)
		free(pair->key);
	if (pair->value)
		free(pair->value);
}

/**
 * @brief Append a new key/value pair to the environment, transferring ownership
 * 		 to env on success.
 * @param env Environment to modify.
 * @param key Key to add; must not be NULL.
 * @param value Value to add; NULL becomes an empty string.
 * @param origin Origin metadata to assign to the new pair.
 * @return 1 on success, 0 on allocation or append failure
 * 		(with internal cleanup).
 */
static int	append_new_pair(t_env *env, char *key, char *value, int origin)
{
	t_epair	pair;
	int		ok;

	if (!env || !key)
		return (0);
	pair.key = 0;
	pair.value = 0;
	pair.origin = 0;
	if (!init_pair_values(&pair, key, value, origin))
	{
		free_pair_values(&pair);
		return (0);
	}
	ok = try_append_epair(env, &pair);
	if (!ok)
	{
		free_pair_values(&pair);
		return (0);
	}
	return (1);
}

/**
 * @brief Set key to value in env with the given origin: update if present,
 * 		otherwise append a new pair.
 * @param env Environment to modify.
 * @param key Key name to set.
 * @param value Value string or NULL for empty.
 * @param origin Origin metadata to store.
 * @return 1 on success, 0 on allocation or API failure.
 */
int	env_set(t_env *env, char *key, char *value, int origin)
{
	int	idx;

	if (!env || !key)
		return (0);
	idx = env_find_key(env, key);
	if (idx >= 0)
		return (update_existing(env, idx, value, origin));
	return (append_new_pair(env, key, value, origin));
}
