/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils8.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 22:39:55 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 21:26:54 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Convert a signed long long to shell status by casting to unsigned
 * 		char (modulo 256).
 * @param v Input value.
 * @return Value in range [0, 255].
 */
int	to_status(long long v)
{
	unsigned char	uc;

	uc = (unsigned char)v;
	return ((int)uc);
}

/**
 * @brief Find the index of a key in env or return -1 if not found.
 * @param env Environment to search.
 * @param key Key name to find.
 * @return Non-negative index if found, -1 otherwise.
 */
int	env_find_key(t_env *env, char *key)
{
	unsigned int	i;

	if (!env || !key)
		return (-1);
	i = 0;
	while (i < env->len)
	{
		if (key && env->pairs[i].key && streq(env->pairs[i].key, key))
			return ((int)i);
		i++;
	}
	return (-1);
}

/**
 * @brief Remove a key from env using remove_epair_by_key, returning its result.
 * @param env Environment to modify.
 * @param key Key name to remove.
 * @return 1 on success, 0 on failure.
 */
int	env_unset(t_env *env, char *key)
{
	if (!env || !key)
		return (0);
	return (remove_epair_by_key(env, key));
}
