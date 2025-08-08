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

// See if it's a heredocument token, if so, tokenize.
int	try_token_heredoc(char **str, t_token_list **list, int *brk)
{
	t_token*	token;

	if (str[0][0] != '<' || str[0][1] != '<')
		return (0);
	if (!create_token(TOKEN_HEREDOC, NULL, &token))
	{
		*brk = 1;
		return (1);
	}
	if (!append_token_list(list, token))
	{
		*brk = 1;
		return (1);
	}
	*str += 2;
	return (1);
}

// See if it's a append redirect token, if so, tokenize.
int	try_token_append_redir(char **str, t_token_list **list, int *brk)
{
	t_token*	token;

	if (str[0][0] != '>' || str[0][1] != '>')
		return (0);
	if (!create_token(TOKEN_REDIRECT_OUTPUT_APPEND, NULL, &token))
	{
		*brk = 1;
		return (1);
	}
	if (!append_token_list(list, token))
	{
		*brk = 1;
		return (1);
	}
	*str += 2;
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
	if (try_token_heredoc(str, list, brk))
		return (1);
	if (*brk == 0)
	{
		if (try_token_append_redir(str, list, brk))
			return (1);
		return (try_1c_tok(str, list, brk, (int[]){'|', TOKEN_PIPE})
			|| try_1c_tok(str, list, brk, (int[]){'<', TOKEN_REDIRECT_INPUT})
			|| try_1c_tok(str, list, brk, (int[]){'>', TOKEN_REDIRECT_OUTPUT}));
	}
	return (0);
}

//
// Is char a whitespace
//
int	is_wspc(char c)
{
	return (c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '\v');
}

//
// Is char a meta character
//
int	is_mtc(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

//? Should this function even have a return value?
//? This will always be the last tokenization attempt, no other possible tokens
//? are possible after this, without next step to skip to, this can only fail.
int	try_word_token(char **str, t_token_list **list, int *brk)
{
	char *s;
	t_token	*token;
	// write(2, "Not implemented!", sizeof "Not implemented!" - 1);
	// _exit(101);

	s = *str;
	while (*s && !is_wspc(*s) && !is_mtc(*s))
	{
		s++;
	}
	// write(1, "WORD TOKENIZED: `", 17);
	// write(1, *str, s - *str);
	// write(1, "`\r\n", 3);
	if (s != *str)
	{
		if (!create_token2(TOKEN_WORD, *str, s - *str, &token))
		{
			*brk = 1;
			return (1);
		}
		*str = s;
		return (*brk = !append_token_list(list, token));
	}
	return (1);
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
		else
			try_word_token(&str, &list, &brk);
	}
	return (1);
}
