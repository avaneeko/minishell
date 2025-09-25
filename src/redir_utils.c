

/* ************************************************************************** */
/*                                                                            */
/*   redir_utils.c                                                            */
/*                                                                            */
/*   Purpose:                                                                 */
/*   - Convert the current redirection array (t_redir **, NULL-terminated)    */
/*     into a proper singly linked list by wiring ->next pointers.            */
/*   - Provide a freeing helper for that array-of-pointers allocation.        */
/*                                                                            */
/*   Rationale (based on the codebase):                                       */
/*   - Builder creates cmd->redirs as t_redir ** and fills entries.           */
/*   - Executor expects t_redir * head and walks via ->next                   */
/*   - Header defines t_redir::next making this chaining accordingly          */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//Convert NULL-terminated t_redir** into a singly linked list.
//* Returns the head (arr[0]) or NULL if no redirections exist.
t_redir	*redir_array_to_list(t_redir **arr)
{
	unsigned int	i;
	t_redir			*head;

	if (!arr)
		return (NULL);
	if (!arr[0])
		return (NULL);
	head = arr[0];
	i = 0;
	while (arr[i])
	{
		if (arr[i + 1])
			arr[i]->next = arr[i + 1];
		else
			arr[i]->next = NULL;
		i++;
	}
	return (head);
}

/* Free the array form (nodes + their pointers array). */
/* Leaves heredoc targets intact if they aren’t heap strings. */
void	free_redir_array(t_redir **arr)
{
	unsigned int	i;
	t_redir			*node;

	i = 0;
	while (arr && arr[i])
	{
		node = arr[i];
		if (node->type != TOKEN_HEREDOC)
			free(node->target);
		free(node);
		i++;
	}
	free(arr);
}
