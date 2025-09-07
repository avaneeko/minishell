#include "minishell.h"
#include <stdlib.h>
#include <string.h>

// Helper function for **create_argv
static unsigned int count_args(t_token_list *list, unsigned int start, unsigned int end)
{
    unsigned int count = 0;
    unsigned int i = start;

    while (i < end)
    {
        if (list->tok[i]->type == TOKEN_WORD)
            count++;
        i++;
    }
    return (count);
}

// Helper function for **create_argv
static char **fill_argv(t_token_list *list, unsigned int start, unsigned int end, unsigned int argc)
{
    char            **argv;
    unsigned int    i = 0;
    unsigned int    j = start;

    argv = malloc(sizeof(char *) * (argc + 1));
    if (!argv)
        return (NULL);
    while (j < end)
    {
        if (list->tok[j]->type == TOKEN_WORD)
        {
            argv[i] = strdup(list->tok[j]->token);
            if (!argv[i])
            {
                while (i > 0)
                    free(argv[--i]);
                free(argv);
                return (NULL);
            }
            i++;
        }
        j++;
    }
    argv[i] = NULL;
    return (argv);
}


// Create argv array of strings from tokens of type TOKEN_WORD in [start, end)
static char **create_argv(t_token_list *list, unsigned int start, unsigned int end)
{
    unsigned int argc;

    argc = count_args(list, start, end);
    return (fill_argv(list, start, end, argc));
}


static t_command	*create_command(t_token_list *list, unsigned int start, unsigned int end)
{
	t_command	*cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->redirs = NULL;
	cmd->is_builtin = 0;
	cmd->next = NULL;
	cmd->argv = create_argv(list, start, end);
	if (!cmd->argv)
	{
		free(cmd);
		return (NULL);
	}
	cmd->redirs = parse_redirections(list, start, end);
	if (!cmd->redirs && start != end) // if no redirections and tokens present, it's ok, else fail
	{
		free_matrix(cmd->argv);
		free(cmd);
		return (NULL);
	}
	if (cmd->argv[0])
    	cmd->is_builtin = is_builtin(cmd->argv[0]);
	else
    	cmd->is_builtin = 0;
	return (cmd);
}

void	free_commands(t_command *head)
{
	t_command	*tmp;

	while (head)
	{
		tmp = head->next;
		if (head->argv)
			free_matrix(head->argv);
		if (head->redirs)
			free_redir_list(head->redirs);
		free(head);
		head = tmp;
	}
}

static int	append_command(t_command **head, t_command **cur, t_command *new_cmd)
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
			if (!append_command(&head, &cur, create_command(list, start, i)))
				return (NULL);
			start = i + 1;
		}
		i++;
	}
	return (head);
}
