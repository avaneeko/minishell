
#include "minishell.h"

static int is_valid_key(char *key)
{
    int i;

    if (!key || !((key[0] >= 'A' && key <= 'Z') || key == '_'
        || (key >= 'a' && key <= 'z')))
        return (0);
    i = 1;
    while (key[i])
    {
        if (!((key[i] >= 'A' && key[i] <= 'Z') || key[i] == '_' ||
            (key[i] >= 'a' && key[i] <= 'z') || (key[i] >= '0' && key[i] <= '9')))
            return (0);
        i++;
    }
    return (1);
}

int builtin_unset(char **argv, t_env *env)
{
    int i;

    i = 1;
    while (argv[i])
    {
        if (is_valid_key(argv[i]))
            env_unset(env, argv[i]);
        else
            write(2, "unset: invalid identifier\n", 26);
        i++;
    }
    return (0);
}
