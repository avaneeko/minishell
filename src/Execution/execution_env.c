/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_env.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:49:02 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/13 23:02:50 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"                  /* t_env, pairs, lengths           */
#include <stdlib.h>                     /* malloc, free                    */
#include "execution_utils.h"
#include "../Builtins/builtins_utils.h"

/* ************************************************************************** */
/*                                                                            */
/*                               execution_env.c                              */
/*                                                                            */
/*   Environment serialization to NULL-terminated array of "KEY=VALUE" pairs. */
/*                                                                            */
/* ************************************************************************** */
/* Build "KEY=VALUE" heap string.  */
// static char	*join_kv(char const *k, char const *v)
// {
// 	size_t	lk;
// 	size_t	lv;
// 	char	*s;

// 	if (!k || !v)
// 		return (NULL);
// 	lk = slen(k);
// 	lv = slen(v);
// 	s = (char *)malloc(lk + 1 + lv + 1);
// 	if (!s)
// 		return (NULL);
// 	mcpy(s, k, lk);
// 	s[lk] = '=';
// 	mcpy(s + lk + 1, v, lv);
// 	s[lk + 1 + lv] = '\0';
// 	return (s);
// }

/* Push one joined KV into out[j], increment j on success.  */
static int	push_kv(char **out, unsigned int *j, char const *k, char const *v)
{
	char	*s;

	if (!k || !v)
		return (1);
	s = join_kv(k, v);
	if (!s)
		return (0);
	out[*j] = s;
	*j = *j + 1;
	return (1);
}

/* Serialize env to a newly allocated NULL-terminated array. */
char	**env_serialize(t_env const *env)
{
	char			**out;
	unsigned int	i;
	unsigned int	j;

	out = (char **)malloc(sizeof(char *) * (env->len + 1));
	if (!out)
		return (NULL);
	i = 0;
	j = 0;
	while (i < env->len)
	{
		if (!push_kv(out, &j, env->pairs[i].key, env->pairs[i].value))
		{
			out[j] = NULL;
			env_free_serialized(out);
			return (NULL);
		}
		i += 1;
	}
	out[j] = NULL;
	return (out);
}
