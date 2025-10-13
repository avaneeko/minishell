/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 13:54:30 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/13 20:03:03 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//$	Implements heredocument.

// From heredoc_expand_prompt.c
int			expand_prompt(t_env const *env, char **input);

// From heredoc_bad_good_heredoc.c
int			is_bad_heredoc(t_app *app, unsigned int i);
int			is_good_heredoc(t_app *app, unsigned int i);

// From heredoc_get_file_name.c
char const	*get_heredoc_filename(void);

// From heredoc_input.c
int			get_heredoc_input(t_app *app, int *fd, char *heredoc_end, int exp);
int			heredoc_input_tty(t_app *app, int *fd, char const *heredoc_end,
				int exp);
int			heredoc_input_fd(t_app *app, int *fd, char const *heredoc_end,
				int exp);

// From heredoc_open_write.c
int			try_open_heredoc(t_app *app, int *fd);
int			write_heredoc(int *fd, char *input, int const nl);

// Used to figure out whether heredoc needs expansion.
static int
	has_quotes(char const *s)
{
	while (*s)
	{
		if (*s == '\'' || *s == '"')
			return (1);
		s++;
	}
	return (0);
}

// heredoc_end - delimiter.
int
	do_prompt(t_app *app, t_token *hd, t_token *hd_end, char *heredoc_end)
{
	int			fd;
	int const	exp = !has_quotes(heredoc_end);

	if (try_open_heredoc(app, &fd) && get_heredoc_input(app, &fd, heredoc_end,
			exp))
	{
		hd->token[0] = app->cur_hd;
		hd_end->type = TOKEN_UNDEFINED;
		return (1);
	}
	if (fd >= 0)
		close(fd);
	if (app->cur_hd_name)
	{
		unlink(app->cur_hd_name);
		free(app->cur_hd_name);
		app->cur_hd_name = 0;
	}
	if (app->cur_hd < 16)
		app->heredocs[app->cur_hd] = -1;
	return (0);
}

static int
	too_many_heredocs(t_app *app)
{
	int unsigned	i;
	int unsigned	count;

	i = ~0;
	count = 0;
	while (++i < app->token_list->len)
	{
		if (app->token_list->tok[i]->type == TOKEN_HEREDOC)
		{
			if (i + 1 >= app->token_list->len)
				return (write(2, "minishell: syntax error, trailing heredoc\n",
						42) != -1);
			else if (app->token_list->tok[i + 1]->type != TOKEN_WORD)
				return (write(2, "minishell: syntax error, unexpected "
						"token after heredoc\n", 56) != -1);
			else if (app->token_list->tok[i + 1]->token[0] == 0)
				return (write(2, "minishell: heredoc followed by an empty "
						"token\n", 46) != -1);
			if (++count > 16)
				return (write(2, "minishell: maximum here-document count "
						"exceeded\n", 48) != -1);
		}
	}
	return (0);
}

int
	prompt_heredoc(t_app *app)
{
	unsigned int	i;

	if (too_many_heredocs(app))
		return (0);
	i = ~0;
	while (++i < app->token_list->len)
	{
		if (is_good_heredoc(app, i))
		{
			if (!do_prompt(app, app->token_list->tok[i],
					app->token_list->tok[i + 1],
					app->token_list->tok[i + 1]->token))
				return (0);
		}
		else if (is_bad_heredoc(app, i))
		{
		}
	}
	return (1);
}
