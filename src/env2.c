/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env2.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:25:31 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/13 14:26:09 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Will free() `env->pairs` on failure.
int	try_append_epair(t_env *env, t_epair const *pair)
{
	if (env->len < env->cap || grow_env(env))
	{
		env->pairs[env->len++] = *pair;
		return (1);
	}
	else
		destroy_epair(pair);
	return (0);
}

// Will free() `env->pairs` on failure.
int	parse_envp(t_env *env, char const **envp)
{
	t_epair	new_pair;

	while (*envp)
	{
		if (create_pair(*envp, &new_pair) && try_append_epair(env, &new_pair))
			envp++;
		else
		{
			free(env->pairs);
			return (0);
		}
	}
	return (1);
}

void	remove_epair_at_idx(t_env *env, unsigned int idx)
{
	destroy_epair(env->pairs + idx);
	env->pairs[idx] = env->pairs[--(env->len)];
}

int	get_epair_by_key(t_env const *env, char const *key, t_epair *out)
{
	unsigned int	i;

	i = ~0u;
	while (++i < env->len)
	{
		if (streq(env->pairs[i].key, key))
		{
			*out = env->pairs[i];
			return (1);
		}
	}
	return (0);
}

int	get_epair_by_key2(t_env const *env, char const *key,
		unsigned int key_len, t_epair *out)
{
	unsigned int	i;
	size_t			pkey_len;

	i = ~0u;
	while (++i < env->len)
	{
		pkey_len = slen(env->pairs[i].key);
		if (pkey_len == key_len && mcmp(env->pairs[i].key, key, key_len) == 0)
		{
			*out = env->pairs[i];
			return (1);
		}
	}
	return (0);
}
