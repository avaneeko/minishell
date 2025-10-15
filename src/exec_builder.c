/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   exec_builder.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/15 18:32:14 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/15 18:37:36 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//$ Purpose: Build the execution plan from the parsed tokens in t_token_list.
// Built execution plan is later sent into the execution itself.
//  The execution plan must built correctly at this stage.
// It may not be modified in the execution itself.

// From exec_handle_redir.c
int		handle_redirect_token(t_command *cmd, t_token *t, char *redir_str,
			int unsigned *cur_redir);

// From syntax_checker.c
int		is_redir(enum e_token_type type);

// From exec_builder1.c
void	handle_word_token(t_command *cmd, t_token *token,
			int unsigned *cur_argv);
void	finalize_redir_list(t_redir **redirs, int unsigned count);
void	finalize_list(t_app *app);
int		count_argv_entries(t_token_list const *list, int unsigned start_idx);
int		count_redir_entries(t_token_list const *list, int unsigned start_idx);

// Mark every token after TOKEN_REDIRECT_* or as TOKEN_UNDEFINED.
// This prevents the redirection targets from being treated as argv entries.
// This is a prepass that must be done before counting argv entries.
// This also prevents malformed commands such as "cat < > file" from being
// treated as valid commands.
static void	token_redirect_prepass(t_token_list *list)
{
	int unsigned	i;

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

// static int handle_token(t_app *app, t_command *cmd, t_cmdarr *cmdarr,
// 		t_token const *token)
// {
// 	if (token->type == TOKEN_WORD)
// 	{
// 		handle_word_token(app, cmd, cmdarr, token);
// 	}
// 	return 1;
// }

// Helper for build_cmd()
// params[0] is cur_argv
// params[1] is cur_redir
static int	cmd_start(t_command *cmd, int unsigned argv_count,
			int unsigned redir_count, int unsigned **params)
{
	cmd->argv = malloc(sizeof(char *) * (argv_count + 1));
	if (!cmd->argv)
		return (0);
	cmd->redirs = malloc(sizeof(t_redir *) * (redir_count + 1));
	if (!cmd->redirs)
	{
		free(cmd->argv);
		return (0);
	}
	cmd->argv[argv_count] = NULL;
	cmd->redirs[redir_count] = NULL;
	*(params[0]) = 0;
	*(params[1]) = 0;
	return (1);
}

// p[0] is cur_argv
// p[1] is cur_redir
// p[2] is list
static int	cmd_loop(t_command *cmd, t_token_list *list, int unsigned *idx,
			int unsigned **p)
{
	t_token	*token;

	while (*idx < list->len)
	{
		token = list->tok[*idx];
		if (token->type == TOKEN_WORD)
			handle_word_token(cmd, token, p[0]);
		else if (is_redir(token->type))
		{
			if (!handle_redirect_token(cmd, token,
					list->tok[*idx + 1]->token, p[1]))
				return (0);
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

static int	build_cmd(t_command *cmd, t_token_list *list, int unsigned *idx)
{
	int unsigned	argv_count;
	int unsigned	redir_count;
	int unsigned	cur_argv;
	int unsigned	cur_redir;

	argv_count = count_argv_entries(list, *idx);
	redir_count = count_redir_entries(list, *idx);
	if (!cmd_start(cmd, argv_count, redir_count,
			(int unsigned *[]){&cur_argv, &cur_redir}))
		return (0);
	cmd_loop(cmd, list, idx,
		(void *)(void *[]){&cur_argv, &cur_redir, (void *)list});
	finalize_redir_list(cmd->redirs, redir_count);
	return (1);
}

int	build_exec(t_app *app)
{
	int unsigned	i;
	t_command		cmd;

	token_redirect_prepass(app->token_list);
	i = 0;
	while (i < app->token_list->len)
	{
		mset(&cmd, 0, sizeof(cmd));
		if (!build_cmd(&cmd, app->token_list, &i))
			return (0);
		if (!cmdarr_append(&app->exec, &cmd, 1))
		{
			return (0);
		}
	}
	finalize_list(app);
	return (1);
}
