#include "minishell.h"

int	is_bad_heredoc(t_app *app, unsigned int i)
{
	if (app->token_list->tok[i]->type == TOKEN_HEREDOC
		&& i + 1 < app->token_list->len
		&& app->token_list->tok[i]->type != TOKEN_WORD)
	{
		// Heredoc followed by non-word.
		// TODO: non-fatal.
		write(2, "FATAL: Heredoc followed by non-word.\r\n", sizeof "FATAL: Heredoc followed by non-word.\r\n");
		_exit(191);
		return (1);
	}
	else if (app->token_list->tok[i]->type == TOKEN_HEREDOC
		&& i + 1 >= app->token_list->len)
	{
		// Heredoc hanging at the end of the list.
		// TODO: non-fatal.
		write(2, "FATAL: Heredoc hanging at the end of the list.\r\n", sizeof "FATAL: Heredoc hanging at the end of the list.\r\n");
		_exit(191);
		return (1);
	}
	return (0);
}

int	is_good_heredoc(t_app *app, unsigned int i)
{
	return (app->token_list->tok[i]->type == TOKEN_HEREDOC
			&& i + 1 < app->token_list->len
			&& app->token_list->tok[i]->type == TOKEN_WORD);
}

// TODO: Make this respect TMPDIR and create heredocs there.
//! Designed for idx bound to 0..15
char const	*get_heredoc_filename(int unsigned idx)
{
	char const *const names[16] = {".hd0", ".hd1", ".hd2", ".hd3", ".hd4",
		".hd5", ".hd6", ".hd7", ".hd8", ".hd9", ".hd10", ".hd11", ".hd12",
		".hd13", ".hd14", ".hd15"};

	return (names[idx]);
}

// Try to open a new heredoc. Opens a file to be used for heredoc.
// idx - heredoc fd.
int	try_open_heredoc(t_app *app, int /*out*/ *idx)
{
	int unsigned	i;

	i = ~0;
	while (++i < 16)
	{
		if (app->heredocs[i] == -1)
			break;
	}
	if (i >= 16)
	{
		write(2, "minishell: maximum here-document count exceeded", 47);
		app_destroy(app);
		exit(1);
	}
	app->heredocs[i] = open(get_heredoc_filename(i), O_RDWR | O_CREAT | O_EXCL,
		0600);
	if (app->heredocs[i] == -1)
		return (0);
	*idx = app->heredocs[i];
	// can't unlink here cuz the files needs to be re-open later.
	return (1);
}

//* will close *fd on failure.
int	get_heredoc_input(t_app *app, int *fd, char const *heredoc_end, int exp)
{
	char	*input;

	if (isatty(STDIN_FILENO))
	{
		// TODO: Signals for readline here.
		input = readline("> ");
		if (!input)
		{
			// CTRL+D
		}
		else if (streq(input, heredoc_end))
		{
			// Found end of heredoc.

		}
		else if (input && exp)
		{

		}
	}
}

int expand_heredoc(t_env const *env, int should_expand)
{
	t_astr	a;

	if (!should_expand)
		return (1);
	if (!astr_create(&a))
		return (0);

	if (!)

	return (1);
}

int do_prompt(t_app *app, char const *heredoc_end)
{
	int	fd;

	if (try_open_heredoc(app, &fd) && get_heredoc_input(app, &fd, heredoc_end,
		exp))
	{
	}
	return (0);
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
		}
		else if (is_bad_heredoc(app, i))
		{

		}
		// Otherwise it has nothing to do with heredoc, do nothing.
		i++;
	}
	return (1);
}
