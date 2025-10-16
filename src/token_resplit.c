/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_resplit.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 16:58:31 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/14 17:00:39 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// From token_resplit1.c
char	toggle_quote(char c, char q);

static int unsigned	split_inner_loop(char const *t, char *q, int unsigned i)
{
	while (t[i])
	{
		*q = toggle_quote(t[i], *q);
		if (!*q && (t[i] == ' ' || t[i] == '\t'))
			break ;
		i++;
	}
	return (i);
}

static int	split(t_token_list **new_list, char const *t)
{
	char			q;
	unsigned int	start;
	unsigned int	i;
	t_token			*new_tok;

	q = 0;
	i = 0;
	while (t[i])
	{
		while (t[i] && !q && (t[i] == ' ' || t[i] == '\t'))
			i++;
		start = i;
		i = split_inner_loop(t, &q, i);
		if (start < i)
		{
			if (create_token2(TOKEN_WORD, t + start, i - start, &new_tok)
				&& append_token_list(new_list, new_tok))
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

static int	do_split(t_token_list *list, t_token_list **new_list)
{
	unsigned int	i;

	i = 0;
	while (i < list->len)
	{
		if (list->tok[i]->type == TOKEN_WORD)
		{
			if (!split(new_list, list->tok[i]->token))
				return (0);
		}
		else if (!append_and_null(new_list, list->tok + i))
			return (0);
		i++;
	}
	return (1);
}

int	token_resplit(t_app *app)
{
	t_token_list	*new_list;

	if (!new_token_list(4096, &new_list))
		return (0);
	if (!do_split(app->token_list, &new_list))
		return (0);
	destroy_token_list_deep(app->token_list);
	app->token_list = new_list;
	return (1);
}
