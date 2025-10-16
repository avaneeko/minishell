/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 14:50:08 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/14 17:21:49 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// From tokenizer1.c
int	try_token_heredoc(char **str, t_token_list **list, int *brk);
int	try_token_append_redir(char **str, t_token_list **list, int *brk);
int	try_1c_tok(char **str, t_token_list **list, int *brk, int param[2]);

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
		return (try_1c_tok(str, list, brk, (int []){'|', TOKEN_PIPE})
			|| try_1c_tok(str, list, brk, (int []){'<', TOKEN_REDIRECT_INPUT})
			|| try_1c_tok(str, list, brk, (int []){'>',
				TOKEN_REDIRECT_OUTPUT}));
	}
	return (0);
}

//
// Is char a meta character
//
int	is_mtc(char c)
{
	return (c == '|' || c == '<' || c == '>');
}

// Walks the string for the quote
// Will set brk if quote is unmatched.
// Returns 1 if we traversed the word, 0 otherwise.
static int	quote_walker(char **str, int *brk)
{
	char const	q = **str;
	char		*s;

	s = *str;
	if (q != '\'' && q != '\"')
		return (0);
	s++;
	while (*s && *s != q)
		s++;
	if (*s == 0)
		return (*brk = 1);
	*str = s + 1;
	return (1);
}

int	tok_word(char **str, t_token_list **list, int *brk)
{
	char	*s;
	t_token	*tok;

	s = *str;
	while (*s && !is_wspc(*s) && !is_mtc(*s) && *brk == 0)
	{
		if (quote_walker(&s, brk))
			continue ;
		else
			s++;
	}
	if (s == *str)
		return (0);
	if (!create_token2(TOKEN_WORD, *str, s - *str, &tok))
		return (*brk = 1);
	if (!append_token_list(list, tok))
		return (*brk = 1);
	*str = s;
	return (1);
}

// 0 on failure.
// cq - Current quote.
int	tokenize(char *str, t_token_list *list)
{
	int	brk;

	brk = 0;
	while (*str && brk == 0)
	{
		while (is_wspc(*str))
			str++;
		if (try_simple_token(&str, &list, &brk))
		{
		}
		else
			tok_word(&str, &list, &brk);
	}
	return (brk == 0);
}
