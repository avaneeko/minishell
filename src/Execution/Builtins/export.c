#include "minishell.h"

static char *find_eq(char *str)
{
    int i;

    i = 0;
    while (str[i])
    {
        if (str[i] == '=')
            return (&str[i]);
        i++;
    }
    return (NULL);
}

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

int builtin_export(char **argv, t_env *env)
{
    int i;
    char *eq_pos;

    i = 1;
    if (!argv[i])
        return (0);
    while (argv[i])
    {
        eq_pos = find_eq(argv[i]);
        if (!is_valid_key(argv[i]) || (eq_pos && eq_pos == argv[i]))
        {
            write(2, "export: invalid identifier\n", 27);
            i++;
            continue;
        }
        if (eq_pos)
            env_set(env, argv[i], eq_pos + 1, 1);
        else
            env_set(env, argv[i], "", 1);
        i++;
    }
    return (0);
}

