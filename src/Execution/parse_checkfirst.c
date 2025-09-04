/*
NEED TO CHECK THIS IF IMPLEMENTATION IS REQUIRED, also refer to Leon's
tokenization.
*/
#include "minishell.h"
#include <stdlib.h>

static char	**create_argv(t_token_list *list,
			unsigned int start, unsigned int count)
{
	char			**argv;
	unsigned int	i;

	argv = malloc(sizeof(char *) * (count + 1));
	if (!argv)
		return (NULL);
	i = 0;
	while (i < count)
	{
		argv[i] = strdup(list->tok[start + i]->token);
		if (!argv[i])
		{
			while (i > 0)
				free(argv[--i]);
			free(argv);
			return (NULL);
		}
		i++;
	}
	argv[count] = NULL;
	return (argv);
}


static unsigned int	count_args(t_token_list *list, unsigned int start,
				unsigned int end)
{
	unsigned int	count;

	count = 0;
	while (start < end)
	{
		if (list->tok[start]->type == TOKEN_WORD)
			count++;
		start++;
	}
	return (count);
}

static t_command	*create_command(t_token_list *list,
					unsigned int start, unsigned int end)
{
	t_command		*cmd;
	unsigned int	argc;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->redirs = NULL;
	cmd->is_builtin = 0;
	cmd->next = NULL;
	argc = count_args(list, start, end);
	cmd->argv = create_argv(list, start, argc);
	if (!cmd->argv)
	{
		free(cmd);
		return (NULL);
	}
	return (cmd);
}

/*
free_commands, append_command, parse; all for one to avoid 25line limit
*/
static void	free_commands(t_command *head)
{
	t_command	*tmp;

	while (head)
	{
		tmp = head->next;
		free_matrix(head->argv);
		free(head);
		head = tmp;
	}
}

static int	append_command(t_command **head, t_command **cur,
				t_command *new_cmd)
{
	if (!new_cmd)
	{
		if (*head)
		{
			free_commands(*head);
			*head = NULL;
		}
		return (0);
	}
	if (!*head)
		*head = new_cmd;
	else
		(*cur)->next = new_cmd;
	*cur = new_cmd;
	return (1);
}

t_command	*parse(t_token_list *list)
{
	t_command		*head;
	t_command		*cur;
	unsigned int	start;
	unsigned int	i;

	if (!list || list->len == 0)
		return (NULL);
	head = NULL;
	cur = NULL;
	start = 0;
	i = 0;
	while (i <= list->len)
	{
		if (i == list->len || list->tok[i]->type == TOKEN_PIPE)
		{
			t_command *cmd = create_command(list, start, i);
			if (!append_command(&head, &cur, cmd))
				return (NULL);
			start = i + 1;
		}
		i++;
	}
	return (head);
}
