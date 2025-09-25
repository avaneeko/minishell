#include "minishell.h"

//$ Purpose: Build the execution plan from the parsed tokens in t_token_list.
// Built execution plan is later sent into the execution itself.
//  The execution plan must built correctly at this stage.
// It may not be modified in the execution itself.

// From exec_handle_redir.c
int	handle_redirect_token(t_command *cmd, t_token *t, char *redir_str,
		int unsigned *cur_redir);

// Mark every token after TOKEN_REDIRECT_* or as TOKEN_UNDEFINED.
// This prevents the redirection targets from being treated as argv entries.
// This is a prepass that must be done before counting argv entries.
// This also prevents malformed commands such as "cat < > file" from being
// treated as valid commands.
static void token_redirect_prepass(t_token_list *list)
{
	int unsigned i;

	i = 0;
	while (i < list->len)
	{
		if (list->tok[i]->type == TOKEN_REDIRECT_INPUT
			|| list->tok[i]->type == TOKEN_REDIRECT_OUTPUT
			|| list->tok[i]->type == TOKEN_REDIRECT_OUTPUT_APPEND
			|| list->tok[i]->type == TOKEN_HEREDOC)
		{
			if (i + 1 < list->len && list->tok[i + 1]->type == TOKEN_WORD)
				list->tok[i + 1]->type = TOKEN_UNDEFINED;
		}
		i++;
	}
}

// TODO: Don't forget to finalize redir list with ->next fields.

static int count_argv_entries(t_token_list const *list, int unsigned start_idx)
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
	return count;
}

static int count_redir_entries(t_token_list const *list, int unsigned start_idx)
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
	return count;
}

// static int handle_token(t_app *app, t_command *cmd, t_cmdarr *cmdarr,
// 		t_token const *token)
// {
// 	if (token->type == TOKEN_WORD)
// 	{
// 		handle_word_token(app, cmd, cmdarr, token);
// 	}
// 	return 1;
// }

static void handle_word_token(t_command *cmd, t_token *token, int unsigned *cur_argv)
{
	// we can afford to reference here, as long as the tokens live during the exec.
	// Simplifies memory management a lot.
	cmd->argv[(*cur_argv)++] = token->token;
}

// TODO: Ensure there are no hanging redirections/heredocs at the end of the token list, this function assumes there are none.
static int build_cmd(t_command *cmd, t_token_list *list, int unsigned *idx)
{
	int unsigned argv_count;
	int unsigned redir_count;
	int unsigned cur_argv;
	int unsigned cur_redir;

	argv_count = count_argv_entries(list, *idx);
	redir_count = count_redir_entries(list, *idx);
	cmd->argv = malloc(sizeof(char *) * (argv_count + 1));
	if (!cmd->argv)
		return 0;
	cmd->redirs = malloc(sizeof(t_redir *) * (redir_count + 1));
	if (!cmd->redirs)
	{
		free(cmd->argv);
		return 0;
	}
	cmd->argv[argv_count] = NULL;
	cmd->redirs[redir_count] = NULL;
	cur_argv = cur_redir = 0;
	while (*idx < list->len)
	{
		t_token *token = list->tok[*idx];
		if (token->type == TOKEN_WORD)
		{
			handle_word_token(cmd, token, &cur_argv);
		}
		else if (token->type == TOKEN_REDIRECT_INPUT
			|| token->type == TOKEN_REDIRECT_OUTPUT
			|| token->type == TOKEN_REDIRECT_OUTPUT_APPEND
			|| token->type == TOKEN_HEREDOC)
		{
			handle_redirect_token(cmd, token, list->tok[*idx + 1]->token, &cur_redir);
		}
		else if (token->type == TOKEN_PIPE)
		{
			(*idx)++;
			break ;
		}
		(*idx)++;
	}
	return (1);
}

//
// Prints the commands contents for debugging purposes.
// Debug only; do not ship.
//
void Debug_PrintCommand(t_command const * cmd)
{
	__builtin_printf("Command argv:\r\n");
	for ( int unsigned i = 0; cmd->argv[i] != NULL; i++ )
	{
		__builtin_printf("  argv[%u]: %s\r\n", i, cmd->argv[i]);
	}
	__builtin_printf("Command redirs:\r\n");
	for ( int unsigned i = 0; cmd->redirs[i] != NULL; i++ )
	{
		if (cmd->redirs[i]->type == TOKEN_HEREDOC)
			__builtin_printf("  redir[%u]: type=%d heredoc_idx=%d\r\n", i, cmd->redirs[i]->type, (int)(uintptr_t)cmd->redirs[i]->target);
		else
			__builtin_printf("  redir[%u]: type=%d target=%s\r\n", i, cmd->redirs[i]->type, cmd->redirs[i]->target);
	}
}

void Debug_PrintCommandArray(t_cmdarr const * arr)
{
	__builtin_printf("CmdArr len=%u cap=%u\r\n", arr->len, arr->cap);
	for ( int unsigned i = 0; i < arr->len; i++ )
	{
		__builtin_printf(" Command #%u:\r\n", i);
		Debug_PrintCommand(&arr->cmds[i]);
	}
}

// Ensures the command list is a linked list via ->next fields.
static void	finalize_list(t_app *app)
{
	int unsigned	i;

	i = ~0;
	while (++i < app->exec.len - 1)
	{
		app->exec.cmds[i].next = &app->exec.cmds[i + 1];
	}
}

int build_exec(t_app *app)
{
	int unsigned	i;
	t_cmdarr		cmdarr;
	t_command		cmd;

	token_redirect_prepass(app->token_list);
	mset(&cmdarr, 0, sizeof(cmdarr));
	if (!cmdarr_create(&cmdarr))
		return 0;
	i = 0;
	while (i < app->token_list->len)
	{
		mset(&cmd, 0, sizeof(cmd));
		if (!build_cmd(&cmd, app->token_list, &i))
		{
			cmdarr_destroy(&cmdarr);
			return 0;
		}
		if (!cmdarr_append(&cmdarr, &cmd, 1))
		{
			cmdarr_destroy(&cmdarr);
			return 0;
		}
	}
	app->exec = cmdarr;
	finalize_list(app);
	/*//! DEBUG */ Debug_PrintCommandArray(&cmdarr); /*//! DEBUG */
	return 1;
}
