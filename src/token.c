/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 13:13:16 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/13 14:20:58 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	create_token(enum e_token_type type, char const *tok, t_token **out)
{
	t_token	*mem;

	if (tok)
		mem = malloc(sizeof(t_token) + (slen(tok) + 1));
	else
		mem = malloc(sizeof(t_token) + 1);
	if (!mem)
		return (0);
	mem->type = type;
	if (tok)
		scpy(mem->token, tok);
	else
		mem->token[0] = 0;
	*out = mem;
	return (1);
}

int	create_token2(enum e_token_type type, char const *tok, size_t tok_len,
		t_token **out)
{
	t_token	*mem;

	if (tok)
		mem = malloc(sizeof(t_token) + (tok_len + 1));
	else
		mem = malloc(sizeof(t_token) + 1);
	if (!mem)
		return (0);
	mem->type = type;
	if (tok)
	{
		mcpy(mem->token, tok, tok_len);
		mem->token[tok_len] = 0;
	}
	else
		mem->token[0] = 0;
	*out = mem;
	return (1);
}

void	destroy_token(t_token const *token)
{
	free((void *)token);
}

int	modify_token(t_token **token, char const *new_contents)
{
	size_t const	len = slen(new_contents);
	t_token			*t;

	if (slen((*token)->token) == len)
	{
		scpy((*token)->token, new_contents);
		return (1);
	}
	else
	{
		t = malloc(sizeof(t_token) + len + 1);
		if (t)
		{
			t->type = (*token)->type;
			scpy(t->token, new_contents);
		}
		free(*token);
		*token = t;
		return (t != 0);
	}
}
