/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exit.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 17:29:55 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/13 22:38:47 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/* Matches bash error text for non-numeric exit arguments. */
static void	print_err_numeric(const char *arg)
{
	print_str(2, "minishell: exit: ");
	print_str(2, arg);
	print_str(2, ": numeric argument required\n");
}

/* Matches bash behavior for too many args: do not exit, status 1.  */
static void	print_err_many(void)
{
	print_str(2, "minishell: exit: too many arguments\n");
}

/* ----------------------- core exit decision logic ----------------------- */
/* Mirrors bash: no args => last code; bad numeric => 2; many args => 1. */
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

/* ---------------------- child and parent entry points -------------------- */
/* Used in child path: compute code; caller will _exit(code). */
int	builtin_exit_child(t_app *app, char **argv)
{
	int	must_exit;
	int	status;

	status = resolve_exit_status(app, argv, &must_exit);
	return (status);
}

/* Used in parent fast-path: print, cleanup, and exit; or keep running. */
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
