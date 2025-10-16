/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 17:29:55 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 00:08:14 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Print error for non-numeric exit argument in bash-compatible format.
 * @param arg The offending argument string.
 */
static void	print_err_numeric(const char *arg)
{
	print_str(2, "minishell: exit: ");
	print_str(2, arg);
	print_str(2, ": numeric argument required\n");
}

/**
 * @brief Print error for too many arguments to 'exit' without exiting
 * 		the shell.
 */
static void	print_err_many(void)
{
	print_str(2, "minishell: exit: too many arguments\n");
}

/**
 * @brief Decide exit status and whether to exit now, matching bash semantics.
 * @param app Application to read last_exit_code when no args.
 * @param argv Arguments for exit; may include a numeric status.
 * @param must_exit Output: set to 1 if shell should exit, 0 if it must
 * 		continue.
 * @return Computed status: last code when no args, 2 for non-numeric, 1 for
 * 		too many args, or normalized numeric status.
 */
static int	resolve_exit_status(t_app *app, char **argv, int *must_exit)
{
	int			argc;
	long long	val;

	*must_exit = 1;
	argc = count_args(argv);
	if (argc == 0)
		return ((int)((unsigned char)app->last_exit_code));
	if (!is_str_numeric(argv[1]) || !parse_ll(argv[1], &val))
	{
		print_err_numeric(argv[1]);
		return (2);
	}
	if (argc > 1)
	{
		print_err_many();
		*must_exit = 0;
		return (1);
	}
	return (to_status(val));
}

/**
 * @brief Child path for 'exit': compute status and return it so caller can
 * 		_exit(status).
 * @param app Application to read last_exit_code if needed.
 * @param argv Arguments for exit.
 * @return Status code to use for _exit.
 */
int	builtin_exit_child(t_app *app, char **argv)
{
	int	must_exit;
	int	status;

	status = resolve_exit_status(app, argv, &must_exit);
	return (status);
}

/**
 * @brief Parent fast-path for 'exit': optionally exit the shell after printing
 * 		'exit' and cleaning up.
 * @param app Application to reset and destroy before exiting.
 * @param argv Arguments for exit.
 * @return If not exiting due to too many args, returns 1, otherwise does not
 * 		return because it calls exit(status).
 */
int	builtin_exit_parent(t_app *app, char **argv)
{
	int	must_exit;
	int	status;

	must_exit = 0;
	status = resolve_exit_status(app, argv, &must_exit);
	if (!must_exit)
		return (status);
	app_reset_exec(app);
	print_str(1, "exit\n");
	app_destroy(app);
	exit(status);
	return (status);
}
