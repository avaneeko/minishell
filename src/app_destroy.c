#include "minishell.h"

void	close_heredocs(t_app *app)
{
	int unsigned	i;

	i = ~0;
	while (++i < 16)
	{
		if (app->heredocs[i] != -1)
			close(app->heredocs[i]);
	}
	mset(app->heredocs, -1, 16);
}

void	app_destroy(t_app *app)
{
    destroy_env(&app->env);
    destroy_token_list_deep(app->token_list);
	close_heredocs(app);
}
