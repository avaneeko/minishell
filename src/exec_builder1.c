/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builder1.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 18:37:46 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/15 18:38:58 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
// we can afford to reference here, as long as the tokens live during the exec.
// Simplifies memory management a lot.
void	handle_word_token(t_command *cmd, t_token *token, int unsigned *cur_argv)
{
	cmd->argv[(*cur_argv)++] = token->token;
}

void	finalize_redir_list(t_redir **redirs, int unsigned count)
{
	int unsigned i;

	if (!count)
		return ;
	i = 0;
	while (i < count - 1)
	{
		redirs[i]->next = redirs[i + 1];
		i++;
	}
	if (count > 0)
		redirs[count - 1]->next = NULL;
}

// Ensures the command list is a linked list via ->next fields.
void	finalize_list(t_app *app)
{
	int unsigned	i;

	i = ~0;
	if (!app->exec.len)
		return ;
	while (++i < app->exec.len - 1)
	{
		app->exec.cmds[i].next = &app->exec.cmds[i + 1];
	}
}

int	count_argv_entries(t_token_list const *list, int unsigned start_idx)
{
	int unsigned i;
	int unsigned count;

	i = start_idx;
	count = 0;
	while (i < list->len)
	{
		if (list->tok[i]->type == TOKEN_PIPE)
			break ;
		if (list->tok[i]->type == TOKEN_WORD)
			count++;
		i++;
	}
	return (count);
}

int	count_redir_entries(t_token_list const *list, int unsigned start_idx)
{
	int unsigned	i;
	int unsigned	count;

	i = start_idx;
	count = 0;
	while (i < list->len)
	{
		if (list->tok[i]->type == TOKEN_REDIRECT_INPUT
			|| list->tok[i]->type == TOKEN_REDIRECT_OUTPUT
			|| list->tok[i]->type == TOKEN_REDIRECT_OUTPUT_APPEND
			|| list->tok[i]->type == TOKEN_HEREDOC)
			count++;
		else if (list->tok[i]->type == TOKEN_PIPE)
			break ;
		i++;
	}
	return (count);
}
