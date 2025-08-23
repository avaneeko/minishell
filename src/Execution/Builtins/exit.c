#include "minishell.h"

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
                exit(255);
            }
            i++;
        }
        exit_status = atoi(argv[1]);
    }
    exit(exit_status);
}
