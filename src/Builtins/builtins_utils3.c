/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils3.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 22:15:54 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 00:23:18 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Return 1 if key exists in the environment, 0 otherwise.
 * @param env Environment to query.
 * @param key Key to search for.
 * @return 1 if found, 0 otherwise.
 */
int	exists_key(t_env *env, char const *key)
{
	t_epair	tmp;

	if (get_epair_by_key(env, key, &tmp))
		return (1);
	return (0);
}

/**
 * @brief Count entries in a NULL-terminated vector of strings.
 * @param v String vector, may be NULL.
 * @return Number of non-NULL entries.
 */
int	count_strv(char **v)
{
	int	n;

	n = 0;
	if (!v)
		return (0);
	while (v[n])
		n += 1;
	return (n);
}

/**
 * @brief Lexicographically compare two strings like strcmp, returning a-b at
 * 		first difference.
 * @param a First string.
 * @param b Second string.
 * @return Negative, zero, or positive per lexicographic order.
 */
int	cmp_str(char const *a, char const *b)
{
	size_t	i;

	i = 0;
	while (a[i] && b[i] && a[i] == b[i])
		i += 1;
	return ((unsigned char)a[i] - (unsigned char)b[i]);
}

/**
 * @brief Swap two char* pointers in-place.
 * @param a Pointer to first element.
 * @param b Pointer to second element.
 */
void	swap_ptr(char **a, char **b)
{
	char	*t;

	t = *a;
	*a = *b;
	*b = t;
}

/**
 * @brief Perform one bubble-sort pass on v of length n and report if any swap
 * 		occurred.
 * @param v Vector of strings to sort.
 * @param n Number of elements.
 * @return 1 if any swap occurred, 0 otherwise.
 */
int	bubble_one_pass(char **v, int n)
{
	int	j;
	int	swapped;

	if (!v || n <= 1)
		return (0);
	j = 0;
	swapped = 0;
	while (j + 1 < n)
	{
		if (cmp_str(v[j], v[j + 1]) > 0)
		{
			swap_ptr(&v[j], &v[j + 1]);
			swapped = 1;
		}
		j += 1;
	}
	return (swapped);
}
