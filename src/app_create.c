#include "minishell.h"

int	app_create(int argc, char const **argv, char const **envp, t_app *out)
{
	(void)argc;
	(void)argv;
	if (!create_env_from_envp(envp, &out->env))
		return (0);
	if (!new_token_list(4096, &out->token_list))
	{
		destroy_env(&out->env);
		return (0);
	}
	mset(out->heredocs, -1, sizeof out->heredocs);

	return 1; // Success!
}
