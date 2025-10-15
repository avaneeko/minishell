/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils4.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 22:04:08 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 00:18:45 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Return slen(s) or 0 if s is NULL to simplify string size computations.
 * @param s Nullable string.
 * @return Length of s or 0 if s is NULL.
 */
size_t	strlen_or_zero(char const *s)
{
	if (s == NULL)
		return (0);
	return (slen(s));
}

/**
 * @brief Copy src bytes to dst starting at *idx and advance *idx by the number
 * 		of bytes copied.
 * @param dst Destination buffer.
 * @param idx In/out: write offset updated by number of bytes copied.
 * @param src Source string to append, ignored if NULL.
 */
void	copy_bytes(char *dst, size_t *idx, char const *src)
{
	size_t	i;

	i = 0;
	if (!src)
		return ;
	while (src[i])
	{
		dst[*idx + i] = src[i];
		i += 1;
	}
	*idx = *idx + i;
}

/**
 * @brief Allocate and build a "KEY=VALUE" string from key and optional value.
 * @param k Non-NULL key string.
 * @param v Nullable value string.
 * @return Newly allocated "KEY=VALUE" string or NULL on failure.
 */
char	*join_kv(char const *k, char const *v)
{
	size_t	kl;
	size_t	vl;
	char	*s;
	size_t	i;

	kl = slen(k);
	vl = strlen_or_zero(v);
	s = (char *)malloc(kl + 1 + vl + 1);
	if (!s)
		return (0);
	i = 0;
	copy_bytes(s, &i, k);
	s[i] = '=';
	i += 1;
	copy_bytes(s, &i, v);
	s[i] = 0;
	return (s);
}

/**
 * @brief Replace an environment key with a new value, creating the pair if
 * 		missing by delegating to append logic.
 * @param env Environment to modify.
 * @param key Key to set.
 * @param val Value string or NULL to set empty string.
 * @return 1 on success, 0 on allocation or API failure.
 */
int	set_env_replace(t_env *env, char const *key, char const *val)
{
	t_epair	pair;
	char	*kv;
	int		ok;

	remove_epair_by_key(env, key);
	kv = join_kv(key, val);
	if (!kv)
		return (0);
	ok = create_pair(kv, &pair);
	free(kv);
	if (!ok)
		return (0);
	if (!try_append_epair(env, &pair))
		return (0);
	return (1);
}

/**
 * @brief Append to an existing key's value or fallback to replace if the key
 * 		does not exist.
 * @param env Environment to modify.
 * @param key Key to append to.
 * @param val Value to append.
 * @return 1 on success, 0 on failure.
 */
int	set_env_append(t_env *env, char const *key, char const *val)
{
	t_epair	cur;
	char	*merged;
	size_t	len;
	size_t	idx;

	if (!get_epair_by_key(env, key, &cur))
		return (set_env_replace(env, key, val));
	len = slen(cur.value) + slen(val);
	merged = (char *)malloc(len + 1);
	if (!merged)
		return (0);
	idx = 0;
	copy_bytes(merged, &idx, cur.value);
	copy_bytes(merged, &idx, val);
	merged[idx] = 0;
	if (!set_env_replace(env, key, merged))
	{
		free(merged);
		return (0);
	}
	free(merged);
	return (1);
}
