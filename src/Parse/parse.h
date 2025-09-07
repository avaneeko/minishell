#ifndef PARSE_H
# define PARSE_H

# include "minishell.h"

// Parses redirections from token list slice [start, end)
t_redir	*parse_redirections(t_token_list *list, unsigned int start, unsigned int end);
void	free_redir_list(t_redir *head);

// Removes quotes from TOKEN_WORD tokens, in-place modification.
// Returns 1 on success, 0 on malloc failure.
int		remove_quotes_tokenlist(t_token_list *list);

// Command list builder from token list (split by TOKEN_PIPE).
t_command	*parse(t_token_list *list);
void		free_commands(t_command *head);

#endif
