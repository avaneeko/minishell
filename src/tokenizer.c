/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 14:50:08 by losypenk          #+#    #+#             */
/*   Updated: 2025/08/01 20:36:28 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// See if it's a pipe token, if so, tokenize.
int	try_token_pipe(char **str, t_token_list **list, int *brk)
{
	t_token*	token;

	if (str[0][0] != '|')
		return (0);
	if (!create_token(TOKEN_PIPE, NULL, &token))
	{
		*brk = 1;
		return (1);
	}
	if (!append_token_list(list, token))
	{
		*brk = 1;
		return (1);
	}
	(*str)++;
	return (1);
}

//	Try 1 character token.
//	Yes, the name was shortened because of norm.
//	Tries to tokenize the single character token if it matches.
//	param[0] - token character.
//	param[1] - token type.
//	For example:
//	try_1char_token(str, list, brk, (int[]){'|', TOKEN_PIPE})
int	try_1c_tok(char **str, t_token_list **list, int *brk,
	int param[2])
{
	t_token*	token;

	if (str[0][0] != param[0])
		return (0);
	if (!create_token(param[1], NULL, &token))
	{
		*brk = 1;
		return (1);
	}
	if (!append_token_list(list, token))
	{
		*brk = 1;
		return (1);
	}
	(*str)++;
	return (1);
}

//	Try to tokenize a simple token, pipe and redirect.
//	brk - break loop, set if an error has occurred.
//	Returns non-zero if a token was picked up or errored.
int	try_simple_token(char **str, t_token_list **list, int *brk)
{
	if (try_1c_tok(str, list, brk, (int[]){'|', TOKEN_PIPE}))
		return (1);
	else if (try_1c_tok(str, list, brk, (int[]){'<', TOKEN_REDIRECT_INPUT}))
		return (1);
	else if (try_1c_tok(str, list, brk, (int[]){'>', TOKEN_REDIRECT_OUTPUT}))
		return (1);
	return (0);
}

int	try_word_token(void)
{
	write(2, "Not implemented!", sizeof "Not implemented!" - 1);
	_exit(101);
}

//
// Is char a whitespace
//
int	is_wspc(char c)
{
	return (c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '\v');
}

// 0 on failure.
// cq - Current quote.
int	tokenize(char *str, t_token_list *list)
{
	int brk;
	// int	cq;

	brk = 0;
	while (*str && brk == 0)
	{
		while (is_wspc(*str))
			str++;
		if (try_simple_token(&str, &list, &brk))
		{
		}
		else if (try_word_token())
		{
			(*str)++;
		}
	}
	return (1);
}
