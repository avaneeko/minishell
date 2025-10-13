/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit_utils3.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 22:15:54 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/13 22:20:00 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/* exists_key: returns 1 if key is present in env.                            */
int	exists_key(t_env *env, char const *key)
{
	t_epair	tmp;

	if (get_epair_by_key(env, key, &tmp))
		return (1);
	return (0);
}

/* ------------------------------- printing path ---------------------------- */
/* count_strv: count serialized env entries.                                  */
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

/* cmp_str: lexicographic compare for sort.                                   */
int	cmp_str(char const *a, char const *b)
{
	size_t	i;

	i = 0;
	while (a[i] && b[i] && a[i] == b[i])
		i += 1;
	return ((unsigned char)a[i] - (unsigned char)b[i]);
}

/* swap_ptr: swap two pointers for bubble sort.                               */
void	swap_ptr(char **a, char **b)
{
	char	*t;

	t = *a;
	*a = *b;
	*b = t;
}

/* bubble_one_pass: one pass of bubble sort and report if swapped.            */
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
