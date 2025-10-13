/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 21:58:36 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/13 23:04:28 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/* print_export_error: matches minishell error style for invalid identifiers. */
void	print_export_error(char const *arg)
{
	print_str(2, "minishell: export: `");
	print_str(2, arg);
	print_str(2, "': not a valid identifier\n");
}

/* --------------------------- identifier utilities ------------------------- */
/* is_name_start: first char must be alpha or underscore.                     */
int	is_name_start(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	if (c == '_')
		return (1);
	return (0);
}

/* is_name_char: subsequent chars may include digits.                         */
int	is_name_char(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	if (c == '_' || (c >= '0' && c <= '9'))
		return (1);
	return (0);
}

/* scan_key_body: walk past a valid NAME, stop at '=' or '+=' or end. */
int	scan_key_body(char const *arg, int start)
{
	int	i;

	i = start;
	while (arg[i] && arg[i] != '=' && !(arg[i] == '+' && arg[i + 1] == '='))
	{
		if (!is_name_char(arg[i]))
			return (-1);
		i += 1;
	}
	return (i);
}

/* parse_key_mode: now short; sets key_len, is_append, has_eq or fails. */
int	parse_key_mode(char const *arg, int *key_len, int *is_append, int *has_eq)
{
	int	i;

	*is_append = 0;
	*has_eq = 0;
	if (!arg || !arg[0] || !is_name_start(arg[0]))
		return (0);
	i = scan_key_body(arg, 1);
	if (i < 0)
		return (0);
	if (arg[i] == '=')
	{
		*key_len = i;
		*has_eq = 1;
		return (1);
	}
	if (arg[i] == '+' && arg[i + 1] == '=')
	{
		*key_len = i;
		*is_append = 1;
		*has_eq = 1;
		return (1);
	}
	*key_len = i;
	return (1);
}
