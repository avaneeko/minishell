#include "minishell.h"
#include <stdlib.h>
#include <string.h>

// Find index of key in env or -1 if not found
int env_find_key(t_env *env, char *key)
{
    unsigned int i;

    i = 0;
    while (i < env->len)
    {
        if (key && env->pairs[i].key && streq(env->pairs[i].key, key))
            return (i);
        i++;
    }
    return (-1);
}

// Add or update key=value in env
int env_set(t_env *env, char *key, char *value, int origin)
{
    int idx;

    idx = env_find_key(env, key);
    if (idx >= 0)
    {
        free(env->pairs[idx].value);
        env->pairs[idx].value = strdup(value);
        env->pairs[idx].origin = origin;
        return (1);
    }
    if (env->len >= env->cap)
        return (0);
    env->pairs[env->len].key = strdup(key);
    env->pairs[env->len].value = strdup(value);
    env->pairs[env->len].origin = origin;
    env->len++;
    return (1);
}

// Remove key from env
int env_unset(t_env *env, char *key)
{
    int idx;
    unsigned int i;

    idx = env_find_key(env, key);
    if (idx < 0)
        return (0);
    free(env->pairs[idx].key);
    free(env->pairs[idx].value);
    i = idx;
    while (i + 1 < env->len)
    {
        env->pairs[i] = env->pairs[i + 1];
        i++;
    }
    env->len--;
    return (1);
}
