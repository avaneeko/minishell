#include "./parse_utils.h"
#include "minishell.h"

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

/*
* Helper function for *create_command
*/
static t_command	*init_command(void)
{
	t_command *cmd;

	cmd = malloc(sizeof(t_command));
	if (!cmd)
		return (NULL);
	cmd->argv = NULL;
	cmd->redirs = NULL;
	cmd->is_builtin = 0;
	cmd->next = NULL;
	return (cmd);
}

/*
* Helper function for *create_command
*/
static int	set_command_argv(t_command *cmd, t_token_list *list,
	unsigned int start, unsigned int end)
{
	cmd->argv = create_argv(list, start, end);
	if (!cmd->argv)
		return (0);
	return (1);
}

// Helper function for "*create_command, free_commands, append_command"
void free_matrix(char **matrix)
{
    int i = 0;

    if (!matrix)
        return;
    while (matrix[i])
    {
        free(matrix[i]);
        i++;
    }
    free(matrix);
}
