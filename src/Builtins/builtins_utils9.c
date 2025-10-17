/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils9.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 21:23:37 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 21:33:06 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Allocate array for printable export lines plus NULL terminator.
 * @param cap Number of entries to reserve space for (excluding the NULL).
 * @return Newly allocated char** of size cap+1 on success, or NULL on failure.
 */
static char	**alloc_export_lines(unsigned int cap)
{
	char	**lines;

	lines = (char **)malloc(sizeof(char *) * (cap + 1));
	return (lines);
}

/**
 * @brief Append one printable entry from env->pairs[i] into lines.
 * @details Uses "KEY" when value is empty, or "KEY=VALUE" when non-empty.
 * @param env Source environment.
 * @param i Index in env->pairs to read.
 * @param lines Destination array to fill.
 * @param[out] n In-out counter of filled entries; incremented on success.
 * @return 1 on success, 0 on allocation error (no increment performed).
 */
static int	add_export_entry(t_env const *env, unsigned int i,
				char **lines, int *n)
{
	char	*s;

	if (!env->pairs[i].key)
		return (1);
	if (env->pairs[i].value && env->pairs[i].value[0] != '\0')
		s = join_kv(env->pairs[i].key, env->pairs[i].value);
	else
		s = ft_strdup(env->pairs[i].key);
	if (!s)
		return (0);
	lines[*n] = s;
	*n += 1;
	return (1);
}

/**
 * @brief Build a NULL-terminated array of printable export lines.
 * @details Populates lines with "KEY" or "KEY=VALUE" entries and returns count.
 * @param env Input environment.
 * @param[out] out_lines Receives allocated array; free with
 * 		env_free_serialized.
 * @param[out] out_n Receives number of entries written.
 * @return 1 on success, 0 on allocation failure (partial buffers are freed).
 */
static int	build_export_lines(t_env const *env, char ***out_lines, int *out_n)
{
	char			**lines;
	unsigned int	i;
	int				n;

	lines = alloc_export_lines(env->len);
	if (!lines)
		return (0);
	i = 0;
	n = 0;
	while (i < env->len)
	{
		if (!add_export_entry(env, i, lines, &n))
		{
			lines[n] = NULL;
			env_free_serialized(lines);
			return (0);
		}
		i += 1;
	}
	lines[n] = NULL;
	*out_lines = lines;
	*out_n = n;
	return (1);
}

/**
 * @brief Print export view in sorted order using 'declare -x' format.
 * @details Shows 'declare -x KEY' for empty values and 'declare -x KEY="VALUE"'
 * 		otherwise.
 * @param env Environment to read and print.
 * @return 1 on success, 0 on allocation/build failure.
 */
int	print_export_noargs(t_env const *env)
{
	char	**lines;
	int		n;
	int		k;

	if (!env)
		return (0);
	if (!build_export_lines(env, &lines, &n))
		return (0);
	sort_lex(lines, n);
	k = 0;
	while (k < n)
	{
		print_one_decl(lines[k]);
		k += 1;
	}
	env_free_serialized(lines);
	return (1);
}
