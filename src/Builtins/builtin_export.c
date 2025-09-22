#include "minishell.h"
#include "builtins_utils.h"
#include <unistd.h>  /* write */

static char	*find_eq(char *str)
{
	int	i;

	i = 0;
	while (str[i])
	{
		if (str[i] == '=')
			return (&str[i]);
		i++;
	}
	return (0);
}

static int	is_valid_key(char *key)
{
	int	i;

	if (!key)
		return (0);
	if (!((key >= 'A' && key <= 'Z')
		|| (key >= 'a' && key <= 'z')
		|| key == '_'))
		return (0);
	i = 1;
	while (key[i])
	{
		if (!((key[i] >= 'A' && key[i] <= 'Z')
			|| (key[i] >= 'a' && key[i] <= 'z')
			|| (key[i] >= '0' && key[i] <= '9')
			|| key[i] == '_'))
			return (0);
		i++;
	}
	return (1);
}

static int	invalid_and_restore(char *eq)
{
	write(2, "export: invalid identifier\n", 27);
	eq = '=';
	return (1);
}

static int	export_pair(char *arg, t_env *env)
{
	char	*eq;

	eq = find_eq(arg);

	if (eq)
	{
		eq = '\0';
		if (!is_valid_key(arg))
			return (invalid_and_restore(eq));
		if (!env_set(env, arg, eq + 1, ORIGIN_EXPORT))
			return (invalid_and_restore(eq));
		eq = '=';
		return (0);
	}
	if (!is_valid_key(arg))
	{
		write(2, "export: invalid identifier\n", 27);
		return (1);
	}
	if (!env_set(env, arg, "", ORIGIN_EXPORT))
		return (1);
	return (0);
}

int	builtin_export(char **argv, t_env *env)
{
	int	i;
	int	err;

	i = 1;
	err = 0;
	if (!argv || !argv[i])
		return (0);
	while (argv[i])
	{
		if (export_pair(argv[i], env))
			err = 1;
		i++;
	}
	return (err);
}
