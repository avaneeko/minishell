/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:14:04 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/07 21:59:00 by jgueon           ###   ########.fr       */
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
		return (0); /* no flag */
	if (s[0] != '-' || s[1] != 'n')
		return (0); /* wrong start */
	i = 2;
	while (s[i] == 'n')
		i++; /* accept any count of 'n' */
	return (s[i] == '\0'); /* reject if any other char appears */
}

int	builtin_echo(char **argv)
{
	int	i;
	int	newline;

	i = 1;
	newline = 1;
	while (argv[i] && is_echo_n(argv[i]))
	{
		newline = 0; /* saw -n style flag */
		i++;
	}
	while (argv[i])
	{
		write(1, argv[i], slen(argv[i])); /* print word  */
		if (argv[i + 1])
			write(1, " ", 1); /* single space between words */
		i++;
	}
	if (newline)
		write(1, "\n", 1); /* default newline when no -n  */
	return (0);
}
