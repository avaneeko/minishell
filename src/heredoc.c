#include "minishell.h"

//
//	Prompt the user for all the here documents inside the tokens.
//
int	prompt_heredoc(t_app *app)
{
	unsigned int	i;

	i = ~0;
	while (++i < app->token_list->len) {
		if (app->token_list->tok[i]->type == TOKEN_HEREDOC)
		{
			// Prompt for heredoc with
			// app->token_list->tok[i] and app->token_list->tok[i + 1]
		}
		// Else do nothing.
	}
	return (1);
}
