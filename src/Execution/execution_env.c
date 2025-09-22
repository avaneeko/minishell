#include "minishell.h"
#include "execution_utils.h"
#include <stdlib.h>

/* Build "key=value". */
static char *join_kv(char const *k, char const *v)
{
    size_t lk;
    size_t lv;
    char   *s;

    lk = slen(k);
    lv = slen(v);
    s = (char *)malloc(lk + 1 + lv + 1);
    if (!s)
        return (NULL);
    mcpy(s, k, lk);
    s[lk] = '=';
    mcpy(s + lk + 1, v, lv);
    s[lk + 1 + lv] = 0;
    return (s);
}

/* Append "k=v" if both exist; update j; return 1 on success, 0 on OOM. */
static int push_kv(char **out, unsigned int *j, char const *k, char const *v)
{
    char *s;

    if (!k || !v)
        return (1);
    s = join_kv(k, v);
    if (!s)
        return (0);
    out[*j] = s;
    *j += 1;
    return (1);
}

/* Serialize env to NULL-terminated array of "KEY=VALUE". */
char **env_serialize(t_env const *env)
{
    char          **out;
    unsigned int    i;
    unsigned int    j;

    out = (char **)malloc(sizeof(char *) * (env->len + 1));
    if (!out)
        return (NULL);
    i = 0;
    j = 0;
    while (i < env->len)
    {
        if (!push_kv(out, &j, env->pairs[i].key, env->pairs[i].value))
        {
            out[j] = NULL;
            env_free_serialized(out);
            return (NULL);
        }
        i++;
    }
    out[j] = NULL;
    return (out);
}
