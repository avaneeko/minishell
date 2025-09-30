#include "minishell.h"

//$ Purpose: Strip quotes from tokens in-place.
//$ This is done after tokenization and before execution, so that the commands
//$ see the unquoted strings.

// This file implements:
void	dequote_tokens(t_app *app);
static void	dequote(t_token * tok);

static void	dequote(t_token * tok)
{
	char	*src;
	char	*dst;
	char	quote;

	src = tok->token;
	dst = tok->token;
	quote = 0;
	while (*src)
	{
		if (*src == '\'' || *src == '\"')
		{
			if (quote == 0)
				quote = *src;
			else if (quote == *src)
				quote = 0;
			else
				*dst++ = *src;
		}
		else
		{
			*dst++ = *src;
		}
		src++;
	}
	*dst = '\0';
}

void	dequote_tokens(t_app *app)
{
	int unsigned	i;

	i = ~0;
	while (++i < app->token_list->len)
	{
		if (app->token_list->tok[i]->type == TOKEN_WORD)
			dequote(app->token_list->tok[i]);
	}
}
