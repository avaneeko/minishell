
#include "minishell.h"    /* t_app, slen, app_destroy, etc. */
#include <unistd.h>       /* write */
#include <stdlib.h>       /* exit */
#include <limits.h>       /* LLONG_MAX, LLONG_MIN [*/

/* ------------------------ printing helpers (unchanged) ------------------- */

/* Safe write helper using project slen() for consistency.  */
static void	print_str(int fd, const char *s)
{
	if (s != NULL)
		write(fd, s, (int)slen(s));
}

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



/* Accept optional sign and then only digits, at least one digit. */
static int	is_str_numeric(const char *s)
{
	int i;

	if (s == NULL || s[0] == '\0')
		return (0);
	i = 0;
	if (s[i] == '+' || s[i] == '-')
		i += 1;
	if (s[i] == '\0')
		return (0);
	while (s[i] != '\0')
	{
		if (s[i] < '0' || s[i] > '9')
			return (0);
		i += 1;
	}
	return (1);
}

/* Extracts optional +/-, sets starting index, rejects empty after sign. */
static int	parse_sign(const char *s, int *idx, int *sign)
{
	*sign = 1;
	*idx = 0;
	if (s[*idx] == '+' || s[*idx] == '-')
	{
		if (s[*idx] == '-')
			*sign = -1;
		*idx += 1;
	}
	if (s[*idx] == '\0')
		return (0);
	return (1);
}

/* For positive: lim = LLONG_MAX; for negative: lim = (unsigned)LLONG_MAX + 1 */
static int	compute_limit_for_sign(int sign, unsigned long long *lim)
{
	if (lim == NULL)
		return (0);
	if (sign == 1)
		*lim = (unsigned long long)LLONG_MAX;
	else
		*lim = (unsigned long long)LLONG_MAX + 1ULL;
	return (1);
}

/* Accumulates one decimal digit with overflow guard under bound 'lim'. */
static int	add_digit_check(unsigned long long *acc, unsigned long long lim, int d)
{
	unsigned long long u;

	if (acc == NULL)
		return (0);
	if (d < 0 || d > 9)
		return (0);
	u = (unsigned long long)d;
	if (*acc > (lim - u) / 10ULL)
		return (0);
	*acc = (*acc * 10ULL) + u;
	return (1);
}

/* Converts accumulated magnitude to signed result, with LLONG_MIN case. */
static void	assign_signed_result(int sign, unsigned long long acc,
		unsigned long long lim, long long *out)
{
	if (out == NULL)
		return ;
	if (sign == 1)
		*out = (long long)acc;
	else if (acc == lim)
		*out = LLONG_MIN;
	else
		*out = -(long long)acc;
}

/* Core digit parser: loop over digits, check overflow per step, then sign. */
static int	parse_digits_core(const char *s, int start, int sign, long long *out)
{
	unsigned long long acc;
	unsigned long long lim;
	int i;
	int d;

	acc = 0;
	if (!compute_limit_for_sign(sign, &lim))
		return (0);
	i = start;
	while (s[i] != '\0')
	{
		d = s[i] - '0';
		if (!add_digit_check(&acc, lim, d))
			return (0);
		i += 1;
	}
	assign_signed_result(sign, acc, lim, out);
	return (1);
}

/* ------------------------ top-level integer parsing ---------------------- */

/* Public helper: parse optional sign, accumulate digits with overflow. */
static int	parse_ll(const char *s, long long *out)
{
	int sign;
	int idx;

	if (!parse_sign(s, &idx, &sign))
		return (0);
	return (parse_digits_core(s, idx, sign, out));
}

/* --------------------------- small arg helpers --------------------------- */

/* Counts non-program arguments for builtin logic. */
static int	count_args(char **argv)
{
	int n;
	int i;

	if (argv == NULL)
		return (0);
	n = 0;
	i = 1;
	while (argv[i] != NULL)
	{
		n += 1;
		i += 1;
	}
	return (n);
}

/* Bash-compatible cast to unsigned char for modulo 256 behavior. */
static int	to_status(long long v)
{
	unsigned char uc;

	uc = (unsigned char)v;
	return ((int)uc);
}

/* ----------------------- core exit decision logic ----------------------- */

/* Mirrors bash: no args => last code; bad numeric => 2; many args => 1. */
static int	resolve_exit_status(t_app *app, char **argv, int *must_exit)
{
	int argc;
	long long val;

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
	int must_exit;
	int status;

	status = resolve_exit_status(app, argv, &must_exit);
	return (status);
}

/* Used in parent fast-path: print, cleanup, and exit; or keep running. */
int	builtin_exit_parent(t_app *app, char **argv)
{
	int must_exit;
	int status;

	status = resolve_exit_status(app, argv, &must_exit);
	if (!must_exit)
	{
		app->last_exit_code = status;
		return (status);
	}
	print_str(1, "exit\n");
	app_destroy(app);
	exit(status);
	return (0);
}
