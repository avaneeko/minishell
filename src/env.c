#include "minishell.h"

int	create_env_from_envp(char const **envp, t_env *out_env)
{
	t_env	env;

	env.pairs = malloc(ENV_MEM_RESERVE * sizeof(t_epair));
	if (!env.pairs)
		return (0);
	env.cap = ENV_MEM_RESERVE;
	env.len = 0;
	if (!parse_envp(&env, envp))
		return (0);
	*out_env = env;
	return (1);
}

void	destroy_env(t_env *env)
{
	unsigned int	i;

	i = ~0u;
	while (++i < env->len)
		destroy_epair(env->pairs + i);
	free(env->pairs);
	env->cap = 0;
	env->len = 0;
	env->pairs = 0;
}

void	destroy_epair(t_epair const *pair)
{
	free(pair->key);
	free(pair->value);
}

//	str - one envp entry.
int		create_pair(char const *str, t_epair *out)
{
	unsigned int const	e = get_char_idx(str, '=');
	t_epair				pair;

	if (e == ~0u)
		return (0);
	pair.key = mclone(str, e + 1);
	if (!pair.key)
		return (0);
	pair.key[e] = 0;
	pair.value = mclone(str + e + 1, slen(str + e + 1));
	if (!pair.value)
	{
		free(pair.key);
		return (0);
	}
	pair.origin = ORIGIN_ENV;
	*out = pair;
	return (1);
}

// Will free() `env->pairs` on failure.
int	grow_env(t_env *env)
{
	t_epair *const	new = mclone_grow(env->pairs, env->len * sizeof(t_epair),
		ENV_MEM_GROW_SIZE * sizeof(t_epair));

	free(env->pairs);
	if (new)
		env->cap += ENV_MEM_GROW_SIZE;
	else
		env->cap = env->len = 0;
	return ((env->pairs = new) != 0);
}

// Will free() `env->pairs` on failure.
static int	try_append(t_env *env, t_epair const *pair)
{
	if (env->len + 1 <= env->cap || grow_env(env))
	{
		env->pairs[env->len++] = *pair;
		return (1);
	}
	else
		destroy_epair(pair);
	return (0);
}

// Will free() `env->pairs` on failure.
int		parse_envp(t_env *env, char const **envp)
{
	t_epair	new_pair;

	while (*envp)
	{
		if (create_pair(*envp, &new_pair) && try_append(env, &new_pair))
			envp++;
		else
		{
			free(env->pairs);
			return (0);
		}
	}
	return (1);
}
