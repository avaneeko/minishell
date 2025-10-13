/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_handle_redir.c                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:19:07 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/13 14:19:25 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//$ Purpose: Handles a redirection token, adding it to the command's
// redirection list.

// This file declares:
int	handle_redirect_token(t_command *cmd, t_token *t, char *redir_str,
		int unsigned *cur_redir);

int	handle_redirect_token(t_command *cmd, t_token *t, char *redir_str,
		int unsigned *cur_redir)
{
	t_redir	*redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (0);
	redir->next = 0;
	redir->type = t->type;
	if (redir->type == TOKEN_HEREDOC)
		redir->target = (char *)(uintptr_t)t->token[0];
	else
		redir->target = redir_str;
	cmd->redirs[(*cur_redir)++] = redir;
	return (1);
}
