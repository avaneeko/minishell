/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_env.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:49:02 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/14 17:02:35 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"
#include "../Builtins/builtins_utils.h"

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
