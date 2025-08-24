#include "minishell.h"

int	app_create(int argc, char const **argv, char const **envp, t_app *out)
{
	(void)argc;
	(void)argv;
	if (!create_env_from_envp(envp, &out->env))
		return 0;

	return 1; // Success!
}
