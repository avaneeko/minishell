#include "minishell.h"

// int	app_create(int argc, char const **argv, char const **envp, t_app *out)
// {
// 	(void)argc;
// 	(void)argv;
// 	if (!create_env_from_envp(envp, &out->env))
// 		return (0);
// 	if (!new_token_list(4096, &out->token_list))
// 	{
// 		destroy_env(&out->env);
// 		return (0);
// 	}
// 	mset(out->heredocs, -1, sizeof out->heredocs);
// 	out->cur_hd_name = NULL;
// 	out->last_exit_code = 0;
// 	out->cur_hd = 0;
// 	out->prompt = NULL;
// 	out->skip_exec = 0;

// 	return 1; // Success!
// }

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
	if (!cmdarr_create(&out->exec))
	{
		destroy_env(&out->env);
		destroy_token_list(out->token_list);
		return (0);
	}
	mset(out->heredocs, -1, sizeof out->heredocs);
	out->cur_hd_name = NULL;
	out->last_exit_code = 0;
	out->cur_hd = 0;
	out->prompt = NULL;
	out->skip_exec = 0;

	return (1); // Success!
}
