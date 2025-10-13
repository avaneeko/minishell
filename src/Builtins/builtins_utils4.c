/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit_utils2.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 22:04:08 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/13 22:16:19 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/* ------------------------------ string helpers ---------------------------- */
/* strlen_or_zero: guards against NULL when building strings.                 */
size_t	strlen_or_zero(char const *s)
{
	if (s == NULL)
		return (0);
	return (slen(s));
}

/* copy_bytes: append src bytes to dst at index pointer.                      */
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

/* join_kv: allocate "KEY=VALUE" string for create_pair.                      */
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

/* --------------------------- environment updates -------------------------- */
/* set_env_replace: remove existing key and set to value.                     */
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

/* set_env_append: key+=val or fallback to replace if key missing.            */
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
