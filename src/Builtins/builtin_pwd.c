/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:20:35 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 00:58:10 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Print the physical current directory resolved from the filesystem
 * 		using getcwd, ignoring symlinks, and return a shell status code.
 * @details "Physical" means the real directory path on disk with all symbolic
 * 		links resolved, which getcwd already provides when called without
 * 		relying on PWD, so this simply fetches and prints that value to stdout.
 * @return 0 on success, non-zero on failure (e.g., getcwd error), after
 * 		printing an error message to stderr.
 */
static int	print_physical(void)
{
	char	*cwd;

	cwd = getcwd(NULL, 0);
	if (!cwd)
	{
		print_str(2, "minishell: pwd: getcwd failed\n");
		return (1);
	}
	print_line(1, cwd);
	free(cwd);
	return (0);
}

/**
 * @brief Print the logical working directory from the PWD environment variable
 * 		if valid, falling back to getcwd when PWD is missing or inconsistent.
 * @details "Logical" means the path the shell has tracked (possibly containing
 * 		symlinks); if PWD is unset or clearly invalid, this falls back to
 * 		printing the physical path from getcwd so that a useful path is still
 * 		printed.
 * @param env Environment to read the "PWD" value from.
 * @return 0 on success, non-zero on failure (e.g., neither PWD nor getcwd can
 * 		provide a path), after printing an error message.
 */
static int	print_logical_or_fallback(t_env *env)
{
	char	*pwd;

	pwd = env_get_value(env, "PWD");
	if (pwd)
	{
		if (print_line(1, pwd))
			return (0);
	}
	return (print_physical());
}

/**
 * @brief Validate a single pwd option token such as "-P" or "-L", updating
 * 		flags accordingly.
 * @details Accepts only clusters of 'P' and 'L' (e.g., "-P", "-L", "-PL",
 * 		 "-LP"), where the last occurrence wins; set *want_physical=1 and
 * 		*want_logical=0 for 'P', or the inverse for 'L', leaving both zero if no
 * 		decision is made.
 * @param s Option token beginning with '-' followed by only 'P' or 'L' letters.
 * @param want_physical Output flag set to 1 when '-P' is selected, otherwise
 * 		left as-is.
 * @param want_logical Output flag set to 1 when '-L' is selected, otherwise
 * 		left as-is.
 * @return 1 if the token is valid, 0 if it contains any character other than
 * 		'P' or 'L'.
 */
static int	validate_pwd_opt(char c, int *is_physical)
{
	if (c == 'P')
	{
		*is_physical = 1;
		return (0);
	}
	if (c == 'L')
	{
		*is_physical = 0;
		return (0);
	}
	print_str(2, "minishell: pwd: invalid option\n");
	return (2);
}

/**
 * @brief Parse pwd options from argv, handling "-P" and "-L"
 * 		(including clustered forms), and return the index of the first
 * 		non-option.
 * @details Starts from argv[1], stops at the first token not starting with '-',
 * 		 or at "--", and applies validate_pwd_opt for each options token; if
 * 		both '-L' and '-P' are provided, the last one seen wins to decide
 * 		behavior.
 * @param argv Argument vector for the builtin, where argv[0] is "pwd".
 * @param first_nonopt Output: index in argv of the first non-option or of
 * 		"--" + 1 if present.
 * @param want_physical Output flag set to 1 when '-P' is selected, 0 otherwise.
 * @param want_logical Output flag set to 1 when '-L' is selected, 0 otherwise.
 * @return 0 on success, 1 if an invalid option is encountered (after printing
 * 		an error to stderr).
 */
static int	parse_pwd_opts(char **argv, int *is_physical)
{
	int	i;
	int	j;
	int	r;

	*is_physical = 0;
	if (!argv || !argv[1] || argv[1][0] != '-')
		return (0);
	i = 1;
	while (argv[i] && argv[i][0] == '-' && argv[i][1])
	{
		j = 1;
		while (argv[i][j])
		{
			r = validate_pwd_opt(argv[i][j], is_physical);
			if (r != 0)
				return (r);
			j += 1;
		}
		break ;
	}
	return (0);
}

/**
 * @brief Print the current working directory to stdout and return a shell
 * 		status code.
 * @param argv Argument vector; accepts no options in the current
 * 		implementation.
 * @param env Environment handle; unused by basic pwd printing.
 * @return 0 on success, non-zero on error obtaining the current directory.
 */
int	builtin_pwd(char **argv, t_env *env)
{
	int	is_physical;
	int	parse_status;

	parse_status = parse_pwd_opts(argv, &is_physical);
	if (parse_status != 0)
		return (parse_status);
	if (is_physical)
		return (print_physical());
	return (print_logical_or_fallback(env));
}
