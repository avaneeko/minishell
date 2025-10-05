// #include "minishell.h"
// #include "builtins_utils.h"

// /**
//  * This function converts the initial portion of the string pointed to by str
//  * to an integer representation. It skips all white-space characters at the
//  * beginning, takes an optional plus or minus sign followed by as many digits
//  * as possible, and interprets them as a numerical value.
//  * ft_atoi - Converts a string to an integer
//  *
//  * @param str: The string to be converted
//  * @return The converted integer value
//  */
// int	ft_atoi(const char *str)
// {
// 	int		sign;
// 	int		result;

// 	sign = 1;
// 	result = 0;
// 	while (*str == ' ' || (*str >= 9 && *str <= 13))
// 		str++;
// 	if (*str == '-' || *str == '+')
// 	{
// 		if (*str == '-')
// 			sign = -1;
// 		str++;
// 	}
// 	while (*str >= '0' && *str <= '9')
// 	{
// 		result = result * 10 + (*str - '0');
// 		str++;
// 	}
// 	return (result * sign);
// }

// int builtin_exit(char **argv)
// {
//     int exit_status;

//     if (!argv[1])
//         exit_status = 0;
//     else
//     {
//         exit_status = 0;
//         int i = 0;
//         while (argv[1][i])
//         {
//             if (argv[1][i] < '0' || argv[1][i] > '9')
//             {
//                 write(2, "exit: numeric argument required\n", 32);
//                 app_destroy(NULL);
//                 exit(255);
//             }
//             i++;
//         }
//         exit_status = ft_atoi(argv[1]);
//     }
//     exit(exit_status);
// }

#include "minishell.h"
#include "builtins_utils.h"
#include <unistd.h>
#include <stdlib.h>

/*
** SIMPLE UTILS:
** - ft_is_space: treat standard ASCII whitespaces as spaces, matching what
** - ft_strlen: used to avoid hardcoding byte counts in write() calls [attached_file:20].
*/

static int	ft_is_space(char c)
{
	if (c == ' ' || (c >= 9 && c <= 13))
		return (1);
	return (0);
}

static size_t	ft_strlen(const char *s)
{
	size_t	len;

	len = 0;
	while (s[len] != '\0')
		len++;
	return (len);
}

/*
** is_numeric_with_sign:
** - Accept optional leading spaces and one optional '+' or '-' followed by
**   digits only, emulating what exit expects for its numeric argument [web:11].
** - Returns 1 if valid, 0 otherwise [web:11].
*/
static int	is_numeric_with_sign(const char *s)
{
	int	i;

	i = 0;
	while (s[i] && ft_is_space(s[i]))
		i++;
	if (s[i] == '+' || s[i] == '-')
		i++;
	if (s[i] < '0' || s[i] > '9')
		return (0);
	while (s[i] >= '0' && s[i] <= '9')
		i++;
	while (s[i] && ft_is_space(s[i]))
		i++;
	return (s[i] == '\0');
}

/*
** to_exit_status:
** - Parse a long long and reduce to the least-significant 8 bits to match
**   shell exit status behavior (0–255) [web:17].
** - This uses simple accumulation with sign; overflow handling can be added
**   if desired, but reducing to 8 bits matches common shell semantics [web:17].
*/
static int	to_exit_status(const char *s)
{
	long long	sign;
	long long	val;

	sign = 1;
	val = 0;
	while (*s && ft_is_space(*s))
		s++;
	if (*s == '+' || *s == '-')
	{
		if (*s == '-')
			sign = -1;
		s++;
	}
	while (*s >= '0' && *s <= '9')
	{
		val = val * 10 + (*s - '0');
		s++;
	}
	val *= sign;
	return ((unsigned char)val);
}

// /*
// ** app_destroy_safe:
// ** - Make cleanup NULL-safe so calling it with NULL never dereferences a null
// **   pointer, preventing segfaults on the error path [web:4][attached_file:20].
// ** - If you already have app_destroy(app), add a NULL check inside it instead.
// */
// static void	app_destroy_safe(t_app *app)
// {
// 	if (app == NULL)
// 		return ;
// 	/* free fields of app here, guarding each as needed [attached_file:20] */
// 	/* ... */
// }

/*
** builtin_exit:
** - Behavior aligned with Bash expectations used as reference by minishell:
**   1) No args: exit with status 0 [attached_file:20].
**   2) One arg:
**      - If not a valid signed number: print "numeric argument required"
**        to stderr and exit with 255 after cleanup [web:11][attached_file:20].
**      - Else exit with status reduced to 0–255 [web:17][attached_file:20].
**   3) More than one arg and first is numeric: print "too many arguments"
**      to stderr, do NOT exit, return 1 so the shell continues [web:7][attached_file:20].
** - IMPORTANT: Never call app_destroy with NULL; use app pointer from state
**   or make destroy function NULL-safe to avoid segfaults [web:4][attached_file:20].
*/
int	builtin_exit(t_app *app, char **argv)
{
	int	argc;

	argc = 0;
	while (argv && argv[argc])
		argc++;
	if (argc == 1)
	{
		app_destroy_safe(app);
		exit(0);
	}
	if (!is_numeric_with_sign(argv[1]))
	{
		write(2, "exit\nminishell: exit: numeric argument required\n",
			ft_strlen("exit\nminishell: exit: numeric argument required\n"));
		app_destroy_safe(app);
		exit(255);
	}
	if (argc > 2)
	{
		write(2, "exit: too many arguments\n",
			ft_strlen("exit: too many arguments\n"));
		return (1);
	}
	app_destroy_safe(app);
	exit(to_exit_status(argv[1]));
}
