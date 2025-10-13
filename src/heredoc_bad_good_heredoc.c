/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_bad_good_heredoc.c                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 16:30:44 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/13 20:00:07 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// This file declares:
int	is_bad_heredoc(t_app *app, unsigned int i);
int	is_good_heredoc(t_app *app, unsigned int i);

// This function used to check if the heredoc at index i is malformed,
// but now it's result is unused, as the syntax checker handles
// heredoc syntax errors.
int	is_bad_heredoc(t_app *app, unsigned int i)
{
	if (app->token_list->tok[i]->type == TOKEN_HEREDOC
		&& i + 1 < app->token_list->len
		&& app->token_list->tok[i + 1]->type != TOKEN_WORD)
	{
		return (1);
	}
	else if (app->token_list->tok[i]->type == TOKEN_HEREDOC
		&& i + 1 >= app->token_list->len)
	{
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
