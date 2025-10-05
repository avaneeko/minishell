/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 15:36:28 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/05 17:56:39 by jgueon           ###   ########.fr       */
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

// int	exec_builtin(char **argv, t_env *env)
// {
// 	if (streq(*argv, "cd"))
// 		return (builtin_cd(argv, env));
// 	 if (streq(*argv, "echo"))
//         return (builtin_echo(argv));
//     if (streq(*argv, "pwd"))
//         return (builtin_pwd());
//     if (streq(*argv, "export"))
//         return (builtin_export(argv, env));
//     if (streq(*argv, "unset"))
//         return (builtin_unset(argv, env));
//     if (streq(*argv, "env"))
//         return (builtin_env(env));
//     if (streq(*argv, "exit"))
//         return (builtin_exit(argv));
//     return (0);
// }

/*
** app_destroy_safe:
** - Make cleanup NULL-safe so calling it with NULL never dereferences a null
**   pointer, preventing segfaults on the error path [web:4][attached_file:20].
** - If you already have app_destroy(app), add a NULL check inside it instead.
*/
void	app_destroy_safe(t_app *app)
{
	if (app == NULL)
		return ;
	/* free fields of app here, guarding each as needed [attached_file:20] */
	/* ... */
}

int	exec_builtin(t_app *app, char **argv, t_env *env)
{
	if (!argv || !argv[0])
		return (0);
	if (streq(argv[0], "cd"))
		return (builtin_cd(argv, env));
	if (streq(argv[0], "echo"))
		return (builtin_echo(argv));
	if (streq(argv[0], "pwd"))
		return (builtin_pwd());
	if (streq(argv[0], "export"))
		return (builtin_export(argv, env));
	if (streq(argv[0], "unset"))
		return (builtin_unset(argv, env));
	if (streq(argv[0], "env"))
		return (builtin_env(env));
	if (streq(argv[0], "exit"))
		return (builtin_exit(app, argv));
	return (0);
}
