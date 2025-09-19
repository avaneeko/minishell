#include "minishell.h"

//$	Implements heredocument.

// From heredoc_expand_prompt.c
int	expand_prompt(t_env const *env, char **input);

// From heredoc_bad_good_heredoc.c
int	is_bad_heredoc(t_app *app, unsigned int i);
int	is_good_heredoc(t_app *app, unsigned int i);

// From heredoc_get_file_name.c
char const	*get_heredoc_filename();

// From heredoc_input.c
int	get_heredoc_input(t_app *app, int *fd, char *heredoc_end, int exp);
int	heredoc_input_tty(t_app *app, int *fd, char const *heredoc_end, int exp);
int	heredoc_input_fd(t_app *app, int *fd, char const *heredoc_end, int exp);

// From heredoc_open_write.c
int	try_open_heredoc(t_app *app, int /*out*/ *fd);
int	write_heredoc(int *fd, char *input, int const nl);

// Used to figure out whether heredoc needs expansion.
static int	has_quotes(char const *s)
{
	while (*s)
	{
		if (*s=='\'' || *s=='"')
			return (1);
		s++;
	}
	return (0);
}

// heredoc_end - delimiter.
int do_prompt(t_app *app, t_token *hd, t_token *hd_end, char *heredoc_end)
{
	int	fd;
	int const exp = !has_quotes(heredoc_end); // is expansion required?

	if (try_open_heredoc(app, &fd) && get_heredoc_input(app, &fd, heredoc_end,
			exp))
	{
		hd->token[0] = app->cur_hd; // Save index of the heredoc fd inside the token.
		hd_end->type = TOKEN_UNDEFINED; // Mark token as spent.
		return (1); // Success!
	}
	else
		return (0); // Failure!
}

int	prompt_heredoc(t_app *app)
{
	unsigned int	i;

	i = ~0;
	while (++i < app->token_list->len) {
		if (is_good_heredoc(app, i))
		{
			// Prompt for heredoc with
			// app->token_list->tok[i] and app->token_list->tok[i + 1]
			if (!do_prompt(app, app->token_list->tok[i],
					app->token_list->tok[i + 1],
					app->token_list->tok[i + 1]->token))
				return (0);
		}
		else if (is_bad_heredoc(app, i))
		{

		}
		// Otherwise it has nothing to do with heredoc, do nothing.
	}
	return (1);
}
