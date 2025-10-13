/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils7.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 22:39:55 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/13 22:46:02 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/* --------------------------- small arg helpers --------------------------- */
/* Bash-compatible cast to unsigned char for modulo 256 behavior. */
int	to_status(long long v)
{
	unsigned char	uc;

	uc = (unsigned char)v;
	return ((int)uc);
}

/* Find index of an existing key or -1 if not found. */
/* - Uses streq() from your utils. */
/* - Returns -1 for NULL env or key. */
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

/* Unset key by delegating to remove_epair_by_key. */
int	env_unset(t_env *env, char *key)
{
	if (!env || !key)
		return (0);
	return (remove_epair_by_key(env, key));
}
