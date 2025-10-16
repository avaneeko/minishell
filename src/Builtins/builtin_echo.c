/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:14:04 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 00:45:15 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Check if a string is a valid -n flag form: "-n", "-nn", "-nnn", etc.
 * @param s String to test.
 * @return 1 if s matches -n with one or more 'n', 0 otherwise.
 */
static int	is_echo_n(char const *s)
{
	int	i;

	if (!s)
		return (0);
	if (s[0] != '-' || s[1] != 'n')
		return (0);
	i = 2;
	while (s[i] == 'n')
		i++;
	return (s[i] == '\0');
}

/**
 * @brief Echo arguments separated by spaces, honoring repeated -n flags to
 * 		suppress the trailing newline.
 * @param argv Vector where argv[0]="echo" and subsequent items are options or
 * 		strings.
 * @return Always returns 0.
 */
int	builtin_echo(char **argv)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	while (argv[i] && is_echo_n(argv[i]))
	{
		newline = 0;
		i++;
	}
	while (argv[i])
	{
		write(1, argv[i], slen(argv[i]));
		if (argv[i + 1])
			write(1, " ", 1);
		i++;
	}
	if (newline)
		write(1, "\n", 1);
	return (0);
}
