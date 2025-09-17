#include "minishell.h"

// This file defines:
int	get_heredoc_input(t_app *app, int *fd, char *heredoc_end, int exp);
int	heredoc_input_tty(t_app *app, int *fd, char const *heredoc_end, int exp);
int	heredoc_input_fd(t_app *app, int *fd, char const *heredoc_end, int exp);
static int	heredoc_stop(t_app *app, void *input, int *fd);
static int	err_reset_heredoc(t_app *app);

// From heredoc_get_file_name.c
char const	*get_heredoc_filename(int unsigned idx);

// From heredoc_expand_prompt.c
int	expand_prompt(t_env const *env, char **input);

// From heredoc_open_write.c
int	try_open_heredoc(t_app *app, int /*out*/ *fd);
int	write_heredoc(int *fd, char *input, int const nl);

//* will close *fd on failure.
int	get_heredoc_input(t_app *app, int *fd, char *heredoc_end, int exp)
{
	unquote_inplace(heredoc_end);

	if (isatty(STDIN_FILENO) && heredoc_input_tty(app, fd, heredoc_end, exp))
		return (1);
	else if (!isatty(STDIN_FILENO)
		&& heredoc_input_fd(app, fd, heredoc_end, exp))
		return (1);
	return err_reset_heredoc(app); // Failure!
}

int	heredoc_input_tty(t_app *app, int *fd, char const *heredoc_end, int exp)
{
	char	*input;

	while (1)
	{
		// TODO [MIN-32]: Signals for readline here.
		input = readline("> ");
		if (!input)
		{
			// CTRL+D
			// Simply stop. Keep whatever was written to the file `*fd`
			// Discard current `input` prompt.
			return heredoc_stop(app, input, fd);
		}
		else if (streq(input, heredoc_end))
		{
			// Found end of heredoc.
			return heredoc_stop(app, input, fd);
		}
		else if (input && exp)
		{
			// Expand and write the input.
			if (!expand_prompt(&app->env, &input)
					|| !write_heredoc(fd, input, 1))
				return (0); // Convey error: OOM during expansion.
		}
		else
		{
			// Simply write the input, no expansion.
			if (!write_heredoc(fd, input, 1))
				return (0);
		}
	}
	return (1);
}

// TODO [MIN-33]: Implement heredoc_input_fd()
int	heredoc_input_fd(t_app *app, int *fd, char const *heredoc_end, int exp)
{
	char *const input = malloc(PROMPT_CHAR_LIMIT + 1);

	if (!input)
		return (0);
	while (1)
	{
	}
	free(input);
	return (1);
}

// Reopens current heredoc and frees the `input` and returns 1.
static int	heredoc_stop(t_app *app, void *input, int *fd)
{
	close(app->heredocs[app->cur_hd]);
	*fd = app->heredocs[app->cur_hd] = open(get_heredoc_filename(app->cur_hd),
		O_RDONLY, 0600);
	unlink(get_heredoc_filename(app->cur_hd));
	free(input);
	return (1);
}

static int	err_reset_heredoc(t_app *app)
{
	write(2, "Fatal: File I/O error during heredocument fd acquisition.\n", 58);
	app_reset_heredocs(app);
	return (0);
}
