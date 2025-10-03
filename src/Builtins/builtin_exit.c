// =================================================
/*
** builtin_exit.c
**
** Goal:
** - For "exit hello 5": print:
**     exit
**     minishell: exit: hello: numeric argument required
**   then terminate with status 255.
**
** Behavior (bash-like, minishell scope):
** - Always print "exit" when actually terminating the shell.
** - If no argument: exit with 0 (or the tracked last status if integrated).
** - If first argument is non-numeric: print error and exit 255 immediately.
** - If first argument is numeric and there is a second argument: print
**   "too many arguments" and do not exit (return 1).
** - If exactly one numeric argument: exit with that value modulo 256.
**/

#include "minishell.h"
#include <unistd.h>     /* write, _exit */

/* ------------- small io helpers ------------- */

static int	ms_strlen(char const *s)
{
	int i;

	i = 0;
	while (s && s[i])
		i++;
	return (i);
}

static void	ms_putstr_fd(char const *s, int fd)
{
	if (s)
		write(fd, s, ms_strlen(s));
}

static void	ms_print_exit(void)
{
	ms_putstr_fd("exit\n", 1);
}

/* ------------- numeric helpers (fast and strict) ------------- */

static int	ms_is_sign(char c)
{
	if (c == '+' || c == '-')
		return (1);
	return (0);
}

/*
** Returns 1 iff s is strictly a numeric token of the form:
**   [ '+' | '-' ] DIGIT{1,}
*/
static int	ms_is_numeric(char const *s)
{
	int i;
	int nd;

	if (!s || !s[0])
		return (0);
	i = 0;
	if (ms_is_sign(s[i]))
		i++;
	nd = 0;
	while (s[i] >= '0' && s[i] <= '9')
	{
		nd++;
		i++;
	}
	if (s[i] != '\0')
		return (0);
	if (nd == 0)
		return (0);
	return (1);
}

/*
** Compute numeric value modulo 256 quickly.
** Assumes ms_is_numeric(s) == 1.
*/
static unsigned char	ms_atoi_mod256(char const *s)
{
	int i;
	int neg;
	int d;
	int acc;

	i = 0;
	neg = 0;
	if (ms_is_sign(s[i]))
	{
		if (s[i] == '-')
			neg = 1;
		i++;
	}
	acc = 0;
	while (s[i] >= '0' && s[i] <= '9')
	{
		d = s[i] - '0';
		acc = ((acc * 10) + d) & 0xFF;
		i++;
	}
	if (neg)
		acc = ((256 - (acc % 256)) & 0xFF);
	return ((unsigned char)acc);
}

/* ------------- builtin exit ------------- */
/*
** Contract:
** - argv[0] == "exit"
** - Print "exit" when leaving the shell.
** - Non-numeric first arg => print error and exit 255 immediately.
** - Numeric first arg with extra arg => print error and do not exit (return 1).
** - One numeric arg => exit with that value modulo 256.
*/
int	builtin_exit(char **argv)
{
	unsigned char	code;

	if (!argv || !argv[1])
	{
		ms_print_exit();
		_exit(0);
	}
	if (!ms_is_numeric(argv[1]))
	{
		ms_print_exit();
		ms_putstr_fd("minishell: exit: ", 2);
		ms_putstr_fd(argv[1], 2);
		ms_putstr_fd(": numeric argument required\n", 2);
		_exit(255);
	}
	if (argv[2])
	{
		ms_putstr_fd("minishell: exit: too many arguments\n", 2);
		return (1);
	}
	code = ms_atoi_mod256(argv[1]);
	ms_print_exit();
	_exit(code);
}
