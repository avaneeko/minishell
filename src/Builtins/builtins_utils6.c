/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils6.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 22:29:02 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 00:27:15 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Write a line s followed by newline to fd, returning 1 on success.
 * @param fd File descriptor to write to.
 * @param s String to write; ignored if NULL.
 * @return 1 if wrote, 0 if s was NULL.
 */
int	print_line(int fd, char const *s)
{
	if (!s)
		return (0);
	write(fd, s, (int)slen(s));
	write(fd, "\n", 1);
	return (1);
}

/**
 * @brief Get the value for a key from the environment, or NULL if missing.
 * @param env Environment to query.
 * @param key Key name to find.
 * @return Pointer to value inside env storage or NULL if not found.
 */
char	*env_get_value(t_env *env, char const *key)
{
	t_epair	p;

	if (!env || !key)
		return (0);
	if (get_epair_by_key(env, key, &p))
		return (p.value);
	return (0);
}

/**
 * @brief Return 1 if string is an optional sign followed by at least one digit
 * 		and only digits.
 * @param s Input string to test.
 * @return 1 if numeric form, 0 otherwise.
 */
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

/**
 * @brief Parse an optional '+' or '-' sign and set index to first digit,
 * 		rejecting empty after sign.
 * @param s Input string.
 * @param idx Output: starting index after the sign.
 * @param sign Output: +1 or -1.
 * @return 1 on success, 0 if no digits follow.
 */
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

/**
 * @brief Count non-program arguments in argv (elements after argv[0]).
 * @param argv Argument vector.
 * @return Number of arguments excluding the program/builtin name.
 */
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
