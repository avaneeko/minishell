#include "minishell.h"
#include <stdlib.h>
#include <string.h>

static int	is_redirection(int type)
{
	return (type == TOKEN_REDIRECT_INPUT
		|| type == TOKEN_REDIRECT_OUTPUT
		|| type == TOKEN_REDIRECT_OUTPUT_APPEND
		|| type == TOKEN_HEREDOC);
}

void	free_redir_list(t_redir *head)
{
	t_redir *tmp;

	while (head)
	{
		tmp = head->next;
		free(head->target);
		free(head);
		head = tmp;
	}
}

static int	append_redir(t_redir **head, t_redir **cur, int type, char *target)
{
	t_redir *redir;

	redir = malloc(sizeof(t_redir));
	if (!redir)
		return (0);
	redir->type = type;
	redir->target = strdup(target);
	if (!redir->target)
	{
		free(redir);
		return (0);
	}
	redir->next = NULL;
	if (!*head)
		*head = redir;
	else
		(*cur)->next = redir;
	*cur = redir;
	return (1);
}

t_redir	*parse_redirections(t_token_list *list, unsigned int start, unsigned int end)
{
	t_redir			*head;
	t_redir			*cur;
	unsigned int	i;

	head = NULL;
	cur = NULL;
	i = start;
	while (i < end)
	{
		if (is_redirection(list->tok[i]->type))
		{
			if (i + 1 >= end || list->tok[i + 1]->type != TOKEN_WORD
				|| !append_redir(&head, &cur, list->tok[i]->type, list->tok[i + 1]->token))
			{
				free_redir_list(head);
				return (NULL);
			}
			i += 2;
		}
		else
			i++;
	}
	return (head);
}
