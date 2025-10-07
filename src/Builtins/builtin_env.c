
#include "minishell.h"
#include "builtins_utils.h"

int builtin_env(t_env *env)
{
    unsigned int i;

    if (!env)
		return (0);
    i = 0;
    while (i < env->len)
    {
        if (env->pairs[i].key && env->pairs[i].value)
        {
            write(1, env->pairs[i].key, slen(env->pairs[i].key));
            write(1, "=", 1);
            write(1, env->pairs[i].value, slen(env->pairs[i].value));
            write(1, "\n", 1);
        }
        i++;
    }
    return (0);
}
