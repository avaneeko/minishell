/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   tokenizer1.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 17:20:27 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/14 17:22:00 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// See if it's a heredocument token, if so, tokenize.
int	try_token_heredoc(char **str, t_token_list **list, int *brk)
{
	t_token	*token;

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
	t_token	*token;

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
int	try_1c_tok(char **str, t_token_list **list, int *brk, int param[2])
{
	t_token	*token;

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
