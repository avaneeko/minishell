#ifndef PARSE_H
# define PARSE_H

# include "minishell.h"
# include <string.h>

// Parses redirections from token list slice [start, end)
t_redir	*parse_redirections(t_token_list *list, unsigned int start, unsigned int end);
void	free_redir_list(t_redir *head);

// Removes quotes from TOKEN_WORD tokens, in-place modification.
// Returns 1 on success, 0 on malloc failure.
int		remove_quotes_tokenlist(t_token_list *list);

// Command list builder from token list (split by TOKEN_PIPE).
t_command	*parse(t_token_list *list);
void		free_commands(t_command *head);

// Helper function for **create_argv(command_parser.c)
static unsigned int count_args(t_token_list *list, unsigned int start, unsigned int end);
static char **fill_argv(t_token_list *list, unsigned int start, unsigned int end, unsigned int argc);

// Helper function for *create_command(command_parser.c)
static t_command	*init_command(void);
static int	set_command_argv(t_command *cmd, t_token_list *list,
	unsigned int start, unsigned int end);

// Helper function for "*create_command, free_commands, append_command"
void free_matrix(char **matrix);

// Libft function replica
char	*ft_strdup(const char *s1);


#endif
