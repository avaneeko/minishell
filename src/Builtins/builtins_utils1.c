/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils1.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 15:36:28 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 00:13:04 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Safe write helper that prints a string to fd if the string is not
 * 		NULL.
 * @param fd File descriptor to write to.
 * @param s Nullable string to print.
 */
void	print_str(int fd, const char *s)
{
	if (s != NULL)
		write(fd, s, (int)slen(s));
}

/**
 * @brief Duplicate a C-string into newly allocated memory, returning NULL on
 * 		allocation failure.
 * @param s1 Source string to duplicate.
 * @return Newly allocated duplicate or NULL if allocation fails.
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

/**
 * @brief Return non-zero if command name is a supported
 * 		builtin (cd, echo, pwd, export, unset, env, exit).
 * @param cmd Command name string.
 * @return 1 if builtin, 0 otherwise.
 */
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

/**
 * @brief Execute a builtin by name and return its status code, handling both
 * 		environment and app state where needed.
 * @param app Application context used by builtins like exit.
 * @param argv Argument vector where argv[0] is the builtin name.
 * @param env Environment for builtins that read or modify variables.
 * @return Status code returned by the builtin, or 0 if argv is empty.
 */
int	exec_builtin(t_app *app, char **argv, t_env *env)
{
	if (!argv || !argv[0])
		return (0);
	if (streq(argv[0], "cd"))
		return (builtin_cd(argv, env));
	if (streq(argv[0], "echo"))
		return (builtin_echo(argv));
	if (streq(argv[0], "pwd"))
		return (builtin_pwd(argv, env));
	if (streq(argv[0], "export"))
		return (builtin_export(argv, env));
	if (streq(argv[0], "unset"))
		return (builtin_unset(argv, env));
	if (streq(argv[0], "env"))
		return (builtin_env(env));
	if (streq(argv[0], "exit"))
		return (builtin_exit_child(app, argv));
	return (0);
}
