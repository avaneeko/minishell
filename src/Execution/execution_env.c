/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_env.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:49:02 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/17 13:20:48 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"
#include "../Builtins/builtins_utils.h"

/**
 * @brief Push a "KEY=VALUE" string into out at index *j and advance the index.
 * @param out Output vector under construction; has capacity for
 * 		env->len + 1 entries.
 * @param j In/out index of the next free slot; incremented on successful push.
 * @param k Key string; when NULL the pair is skipped but success is reported
 * 		to continue building.
 * @param v Value string; when NULL the pair is skipped but success is reported
 *		to continue building.
 * @return 1 on success (including skipped pairs when k or v is NULL), 0 on
 *		allocation failure.
 * @note Used by env_serialize to build the execve-compatible environment
 *		vector.
 */
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

/**
 * @brief Serialize the internal environment into a newly allocated
 * 			NULL-terminated char** suitable for execve.
 * @param env Environment structure containing key/value pairs.
 * @return Newly allocated vector of "KEY=VALUE" strings ending with NULL, or
 * 			NULL on allocation failure.
 */
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
		if (env->pairs[i].key && env->pairs[i].value
			&& env->pairs[i].value[0] != '\0')
		{
			if (!push_kv(out, &j, env->pairs[i].key, env->pairs[i].value))
			{
				out[j] = NULL;
				env_free_serialized(out);
				return (NULL);
			}
		}
		i += 1;
	}
	out[j] = NULL;
	return (out);
}
