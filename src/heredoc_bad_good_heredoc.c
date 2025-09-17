#include "minishell.h"

// This file declares:
int	is_bad_heredoc(t_app *app, unsigned int i);
int	is_good_heredoc(t_app *app, unsigned int i);

int	is_bad_heredoc(t_app *app, unsigned int i)
{
	if (app->token_list->tok[i]->type == TOKEN_HEREDOC
		&& i + 1 < app->token_list->len
		&& app->token_list->tok[i + 1]->type != TOKEN_WORD)
	{
		// Heredoc followed by non-word.
		// TODO: non-fatal.
		write(2, "FATAL: Heredoc followed by non-word.\r\n", sizeof "FATAL: Heredoc followed by non-word.\r\n");
		_exit(191);
		return (1);
	}
	else if (app->token_list->tok[i]->type == TOKEN_HEREDOC
		&& i + 1 >= app->token_list->len)
	{
		// Heredoc hanging at the end of the list.
		// TODO: non-fatal.
		write(2, "FATAL: Heredoc hanging at the end of the list.\r\n", sizeof "FATAL: Heredoc hanging at the end of the list.\r\n" - 1);
		_exit(191);
		return (1);
	}
	return (0);
}

int	is_good_heredoc(t_app *app, unsigned int i)
{
	return (app->token_list->tok[i]->type == TOKEN_HEREDOC
			&& i + 1 < app->token_list->len
			&& app->token_list->tok[i + 1]->type == TOKEN_WORD);
}
