#include "minishell.h"
#include "builtins_utils.h"

/**
 * This function converts the initial portion of the string pointed to by str
 * to an integer representation. It skips all white-space characters at the
 * beginning, takes an optional plus or minus sign followed by as many digits
 * as possible, and interprets them as a numerical value.
 * ft_atoi - Converts a string to an integer
 *
 * @param str: The string to be converted
 * @return The converted integer value
 */
int	ft_atoi(const char *str)
{
	int		sign;
	int		result;

	sign = 1;
	result = 0;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		result = result * 10 + (*str - '0');
		str++;
	}
	return (result * sign);
}

int builtin_exit(char **argv)
{
    int exit_status;

    if (!argv[1])
        exit_status = 0;
    else
    {
        exit_status = 0;
        int i = 0;
        while (argv[1][i])
        {
            if (argv[1][i] < '0' || argv[1][i] > '9')
            {
                write(2, "exit: numeric argument required\n", 32);
                app_destroy(NULL);
                exit(255);
            }
            i++;
        }
        exit_status = ft_atoi(argv[1]);
    }
    exit(exit_status);
}
