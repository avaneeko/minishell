/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils5.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 22:29:02 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/13 22:43:30 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

int	print_line(int fd, char const *s)
{
	if (!s)
		return (0);
	write(fd, s, (int)slen(s));
	write(fd, "\n", 1);
	return (1);
}

/* -------- env helper -------- */
char	*env_get_value(t_env *env, char const *key)
{
	t_epair	p;

	if (!env || !key)
		return (0);
	if (get_epair_by_key(env, key, &p))
		return (p.value);
	return (0);
}

/* Accept optional sign and then only digits, at least one digit. */
int	is_str_numeric(const char *s)
{
	int	i;

	if (s == NULL || s[0] == '\0')
		return (0);
	i = 0;
	if (s[i] == '+' || s[i] == '-')
		i += 1;
	if (s[i] == '\0')
		return (0);
	while (s[i] != '\0')
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i += 1;
	}
	return (1);
}

/* Extracts optional +/-, sets starting index, rejects empty after sign. */
int	parse_sign(const char *s, int *idx, int *sign)
{
	*sign = 1;
	*idx = 0;
	if (s[*idx] == '+' || s[*idx] == '-')
	{
		if (s[*idx] == '-')
			*sign = -1;
		*idx += 1;
	}
	if (s[*idx] == '\0')
		return (0);
	return (1);
}

/* Counts non-program arguments for builtin logic. */
int	count_args(char **argv)
{
	int	n;
	int	i;

	if (argv == NULL)
		return (0);
	n = 0;
	i = 1;
	while (argv[i] != NULL)
	{
		n += 1;
		i += 1;
	}
	return (n);
}
