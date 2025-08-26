#include "minishell.h"

void	app_destroy(t_app *app)
{
    destroy_env(&app->env);
    destroy_token_list_deep(app->token_list);
}