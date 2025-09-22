#include "minishell.h"
#include "builtins_utils.h"

int	env_find_key(t_env *env, char *key)
{
	unsigned int	i;

	i = 0;
	while (i < env->len)
	{
		if (key && env->pairs[i].key && streq(env->pairs[i].key, key))
			return ((int)i);
		i++;
	}
	return (-1);
}

static int	update_existing(t_env *env, int idx, char *value, int origin)
{
	char	*dup;

	dup = ft_strdup(value);
	if (!dup)
		return (0);
	free(env->pairs[idx].value);
	env->pairs[idx].value = dup;
	env->pairs[idx].origin = origin;
	return (1);
}

static int	append_new_pair(t_env *env, char *key, char *value, int origin)
{
	t_epair	pair;

	pair.key = ft_strdup(key);
	pair.value = ft_strdup(value);
	pair.origin = origin;
	if (!pair.key || !pair.value)
	{
		if (pair.key)
			free(pair.key);
		if (pair.value)
			free(pair.value);
		return (0);
	}
	if (!try_append_epair(env, &pair))
		return (0);
	return (1);
}

int	env_set(t_env *env, char *key, char *value, int origin)
{
	int	idx;

	idx = env_find_key(env, key);
	if (idx >= 0)
		return (update_existing(env, idx, value, origin));
	return (append_new_pair(env, key, value, origin));
}

int	env_unset(t_env *env, char *key)
{
	return (remove_epair_by_key(env, key));
}
