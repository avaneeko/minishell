#include "minishell.h"

// From signals.c
void	set_default_signals(void);
void	set_heredoc_signals(void);

t_app *get_app(void)
{
	static t_app app;
	return (&app);
}

static void reset(t_app * app)
{
	clear_token_list(app->token_list);
	app_reset_heredocs(app);
	app_reset_exec(app);
	free(app->prompt);
	app->prompt = NULL;
	free(app->pids);
	app->pids = NULL;
	app->skip_exec = 0;
}

int	main(int argc, char const *argv[], char const *envp[])
{
	t_app *app = get_app();

	if (!app_create(argc, argv, envp, app))
		return EXIT_FAILURE;
	while (prompt(app) > 0)
	{
		if (!tokenize(app->prompt, app->token_list))
		{
			write(1, "Tokenizer error.\n", sizeof "Tokenizer error.\n" - 1);
			free(app->prompt);
			continue;
		}
		if (!prompt_heredoc(app))
		{
			reset(app);
			continue ;
		}
		expand(app, &app->token_list, &app->env);
		token_resplit(app);
		dequote_tokens(app);
		if (is_syntax_valid(app) && !app->skip_exec)
		{
			build_exec(app);
			if (app->exec.len)
				app->last_exit_code = execute_pipeline(app, &app->exec.cmds[0], &app->env);
			else
				app->last_exit_code = execute_pipeline(app, NULL, &app->env);
		}
		reset(app);
	}
	app_destroy(app);
	return (app->last_exit_code);
}
