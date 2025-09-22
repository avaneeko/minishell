/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 15:36:28 by jgueon            #+#    #+#             */
/*   Updated: 2025/09/16 16:58:19 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Duplicates a string
 *
 * Description: This function allocates memory for a new string which is
 * a duplicate of the string s. Memory for the new string is obtained with
 * malloc, and can be freed with free.
 *
 * @param s: The string to duplicate
 *
 * @return A pointer to the duplicated string, or NULL if insufficient memory
 * was available
 */
char	*ft_strdup(const char *s1)
{
	char	*dup;
	int		len;
	int		i;

	len = 0;
	while (s1[len])
		len++;
	dup = (char *)malloc(sizeof(char) * (len + 1));
	if (dup == NULL)
		return (NULL);
	i = 0;
	while (i < len)
	{
		dup[i] = s1[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}

/****************************************************************************
 *  Check if command is a builtin, and one to execute the detected built in *
 *																			*
 ****************************************************************************/
int	is_builtin(const char *cmd)
{
	return (
		streq(cmd, "cd")
		|| streq(cmd, "echo")
		|| streq(cmd, "pwd")
		|| streq(cmd, "export")
		|| streq(cmd, "unset")
		|| streq(cmd, "env")
		|| streq(cmd, "exit")
	);
}

int	exec_builtin(char **argv, t_env *env)
{
	if (streq(*argv, "cd"))
		return (builtin_cd(argv, env));
	 if (streq(*argv, "echo"))
        return (builtin_echo(argv));
    if (streq(*argv, "pwd"))
        return (builtin_pwd());
    if (streq(*argv, "export"))
        return (builtin_export(argv, env));
    if (streq(*argv, "unset"))
        return (builtin_unset(argv, env));
    if (streq(*argv, "env"))
        return (builtin_env(env));
    if (streq(*argv, "exit"))
        return (builtin_exit(argv));
    return (0);
}
