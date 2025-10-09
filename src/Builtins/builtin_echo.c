/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:14:04 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/09 17:25:24 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/* minishell: echo must accept -n, -nnn, and repeated -n flags like bash */
/* - Rule: A flag is valid if it is '-' followed by one or more 'n' and nothing else. */
/* - Consume all leading valid flags, then print args separated by one space; omit trailing '\n' if any -n was seen. */
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
