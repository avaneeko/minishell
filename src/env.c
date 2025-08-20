#include "minishell.h"

int	create_env_from_envp(char const **envp, t_env **out_env)
{
	t_env	*env;

	env = malloc(sizeof(t_env) + ENV_MEM_RESERVE * sizeof(t_epair));
	if (!env)
		return (0);
	env->len = 0;
	env->cap = ENV_MEM_RESERVE;

	if (parse_envp(env, envp))
		return ((*out_env = env) != 0);
}

void	destroy_env(t_env const *env)
{
	unsigned int	i;

	i = 0;
	while (i < env->len)
		destroy_epair(&env->pairs[i++]);
	free((void *)env);
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


	if (e == ~0)
		return (0);
	pair.key = mclone(str, e);
	if (!pair.key)
		return (0);
	pair.key[e] = 0;
	pair.value = mclone(str + e, slen(str + e));
	*out = pair;
	return (1);
}

int		parse_envp(t_env *env, char const **envp)
{
	while (*envp)
	{

	}
	free(env);
}
