#include "minishell.h"

//$ Purpose: Handles a redirection token, adding it to the command's
// redirection list.

// This file declares:
int	handle_redirect_token(t_command *cmd, t_token *t, char *redir_str,
		int unsigned *cur_redir);

int	handle_redirect_token(t_command *cmd, t_token *t, char *redir_str,
		int unsigned *cur_redir)
{
	t_redir *redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (0);
	redir->next = 0;
	redir->type = t->type;
	if (redir->type == TOKEN_HEREDOC)
		redir->target = (char *)(uintptr_t)t->token[0]; // Has the heredoc fd in ptr.
	else
		redir->target = redir_str; // Filepath
	cmd->redirs[(*cur_redir)++] = redir;
	return (1);
}
