#include "minishell.h"

char toggle_quote(char c, char q)
{
	if ((c == '\'' || c == '\"') && q == 0)
		return c; // open
	if (q != 0 && c == q)
		return 0; // close
	return q; // unchanged
}

static int split(t_token_list **new_list, char const *t)
{
	char q;
	unsigned int start;
	unsigned int i;
	
	q = 0;
	i = 0;
	while (t[i])
	{
		while (t[i] && !q && (t[i] == ' ' || t[i] == '\t')) // Skip leading spaces outside quotes
			i++;
		start = i;
		while (t[i]) // Find end of token (space outside quotes or end of string)
		{
			q = toggle_quote(t[i], q);
			if (!q && (t[i] == ' ' || t[i] == '\t'))
				break;
			i++;
		} if (start < i)
		{
			t_token *new_tok; // Create new TOKEN_WORD from t[start..i-1]
			if (create_token2(TOKEN_WORD, t + start, i - start, &new_tok) && append_token_list(new_list, new_tok))
				;
			else
				return (0);
		}
	}
	return (1);
}

// useful when moving elements from one list into another without reallocation.
// returns zero on failure.
static int	append_and_null(t_token_list **list, t_token **tok)
{
	int const	success = append_token_list(list, *tok);

	*tok = 0;
	return (success);
}

static int do_split(t_token_list *list, t_token_list **new_list)
{
	unsigned int i;

	i = 0;
	while (i < list->len)
	{
		if (list->tok[i]->type == TOKEN_WORD)
		{
			if(!split(new_list, list->tok[i]->token))
				return (0);
		}
		else if (!append_and_null(new_list, list->tok + i))
			return (0);
		i++;
	}
	return (1);
}

int token_resplit(t_app *app)
{
	t_token_list *new_list;

	if (!new_token_list(4096, &new_list))
		return (0);
	if (!do_split(app->token_list, &new_list))
		return (0);
	destroy_token_list_deep(app->token_list);
	app->token_list = new_list;
	return (1);
}