/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   syntax_checker.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:40:29 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/13 15:59:07 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//$ Purpose: Check the syntax of the parsed tokens in t_token_list.
// Returns 1 if syntax is valid, 0 if invalid with the error printed to stderr.
// Note that the helper functions return the opposite, 1 on error, 0 on success.

static int	is_redir(enum e_token_type type)
{
	return (type == TOKEN_REDIRECT_INPUT
		|| type == TOKEN_REDIRECT_OUTPUT
		|| type == TOKEN_REDIRECT_OUTPUT_APPEND
		|| type == TOKEN_HEREDOC);
}

static int	consecutive_redirections(t_app *app)
{
	int unsigned	i;

	i = ~0;
	while (++i < app->token_list->len - 1)
	{
		if (is_redir(app->token_list->tok[i]->type)
			&& is_redir(app->token_list->tok[i + 1]->type))
		{
			write(2,
				"minishell: syntax error near unexpected token `newline'\n", 56)
			;
			app->last_exit_code = 2;
			return (1);
		}
	}
	return (0);
}

static int	redirection_without_target(t_app *app)
{
	int unsigned	i;

	i = ~0;
	while (++i < app->token_list->len)
	{
		if (is_redir(app->token_list->tok[i]->type))
		{
			if (i + 1 >= app->token_list->len
				|| (app->token_list->tok[i + 1]->type != TOKEN_WORD
					&& app->token_list->tok[i + 1]->type != TOKEN_UNDEFINED))
			{
				write(2, "minishell: syntax error near unexpected token"
					" `newline'\n", 56);
				app->last_exit_code = 2;
				return (1);
			}
		}
	}
	return (0);
}

static int	consecutive_pipes(t_app *app)
{
	int unsigned	i;

	i = ~0;
	while (++i < app->token_list->len - 1)
	{
		if (app->token_list->tok[i]->type == TOKEN_PIPE
			&& app->token_list->tok[i + 1]->type == TOKEN_PIPE)
		{
			write(2, "minishell: syntax error near unexpected token `|'\n", 50);
			app->last_exit_code = 2;
			return (1);
		}
	}
	return (0);
}

int	is_syntax_valid(t_app *app)
{
	if (app->token_list->len == 0)
		return (1);
	if (app->token_list->tok[0]->type == TOKEN_PIPE)
	{
		write(2, "minishell: syntax error near unexpected token `|'\n", 50);
		app->last_exit_code = 2;
		return (0);
	}
	if (app->token_list->tok[app->token_list->len - 1]->type == TOKEN_PIPE)
	{
		write(2, "minishell: syntax error near unexpected token `|'\n", 50);
		app->last_exit_code = 2;
		return (0);
	}
	if (consecutive_redirections(app) || redirection_without_target(app)
		|| consecutive_pipes(app))
		return (0);
	return (1);
}
