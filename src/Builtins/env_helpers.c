#include "minishell.h"
#include "builtins_utils.h"

/* Find index of an existing key or -1 if not found. */
/* - Uses streq() from your utils. */
/* - Returns -1 for NULL env or key. */
int	env_find_key(t_env *env, char *key)
{
	unsigned int	i;

	if (!env || !key)
		return (-1);
	i = 0;
	while (i < env->len)
	{
		if (key && env->pairs[i].key && streq(env->pairs[i].key, key))
			return ((int)i);
		i++;
	}
	return (-1);
}

/* Update value for an existing key. */
/* - Duplicates value; if value is NULL, stores empty string. */
/* - Keeps origin in sync. */
static int	update_existing(t_env *env, int idx, char *value, int origin)
{
	char	*dup;

	if (!env || idx < 0 || idx >= (int)env->len)
		return (0);
	if (value == NULL)
		dup = ft_strdup("");
	else
		dup = ft_strdup(value);
	if (!dup)
		return (0);
	free(env->pairs[idx].value);
	env->pairs[idx].value = dup;
	env->pairs[idx].origin = origin;
	return (1);
}

/* helper function for append_new_pair */
static int	init_pair_values(t_epair *pair, char *key, char *value, int origin)
{
	if (!pair || !key)
		return (0);
	pair->key = ft_strdup(key);
	if (value)
		pair->value = ft_strdup(value);
	else
		pair->value = ft_strdup("");
	pair->origin = origin;
	if (!pair->key || !pair->value)
		return (0);
	return (1);
}

/* helper function for append_new_pair */
static void	free_pair_values(t_epair *pair)
{
	if (!pair)
		return ;
	if (pair->key)
		free(pair->key);
	if (pair->value)
		free(pair->value);
}
/* Append a new key/value pair. */
/* - Allocates key/value strings; frees them on any failure to avoid leaks. */
/* - Delegates growth/ownership to try_append_epair on success. */
static int	append_new_pair(t_env *env, char *key, char *value, int origin)
{
	t_epair	pair;
	int		ok;

	if (!env || !key)
		return (0);
	pair.key = 0;
	pair.value = 0;
	pair.origin = 0;
	if (!init_pair_values(&pair, key, value, origin))
	{
		free_pair_values(&pair);
		return (0);
	}
	ok = try_append_epair(env, &pair);
	if (!ok)
	{
		free_pair_values(&pair);
		return (0);
	}
	return (1);
}

/* Set key to value with origin. */
/* - Updates if present, otherwise appends. */
/* - Returns 1 on success, 0 on allocation or API failure. */
int	env_set(t_env *env, char *key, char *value, int origin)
{
	int	idx;

	if (!env || !key)
		return (0);
	idx = env_find_key(env, key);
	if (idx >= 0)
		return (update_existing(env, idx, value, origin));
	return (append_new_pair(env, key, value, origin));
}

/* Unset key by delegating to remove_epair_by_key. */
int	env_unset(t_env *env, char *key)
{
	if (!env || !key)
		return (0);
	return (remove_epair_by_key(env, key));
}
