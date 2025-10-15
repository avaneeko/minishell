/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils2.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 21:58:36 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 00:15:41 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Print minishell-style export error for an invalid identifier.
 * @param arg The invalid argument string to report.
 */
void	print_export_error(char const *arg)
{
	print_str(2, "minishell: export: `");
	print_str(2, arg);
	print_str(2, "': not a valid identifier\n");
}

/**
 * @brief Return 1 if the first identifier character is alphabetic or
 * 		underscore.
 * @param c Character to test.
 * @return 1 if valid start character, 0 otherwise.
 */
int	is_name_start(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	if (c == '_')
		return (1);
	return (0);
}

/**
 * @brief Return 1 if the identifier body character is alphanumeric or
 * 		underscore.
 * @param c Character to test.
 * @return 1 if valid identifier body character, 0 otherwise.
 */
int	is_name_char(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	if (c == '_' || (c >= '0' && c <= '9'))
		return (1);
	return (0);
}

/**
 * @brief Scan identifier body until '=' or '+=' or end, ensuring valid
 * 		characters.
 * @param arg Full argument string.
 * @param start Index to begin scanning from.
 * @return Index of the stopping position, or -1 on invalid character.
 */
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

/**
 * @brief Parse an export argument to determine key length, whether '+=' append
 * 		mode is used, and if '=' is present.
 * @param arg Input argument like KEY, KEY=VAL, or KEY+=VAL.
 * @param key_len Output: length of the key.
 * @param is_append Output: 1 if '+=' mode detected, else 0.
 * @param has_eq Output: 1 if '=' present, else 0.
 * @return 1 on success, 0 on invalid identifier.
 */
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
