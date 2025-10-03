#include "minishell.h"

// This file defines:
int	get_heredoc_input(t_app *app, int *fd, char *heredoc_end, int exp);
int	heredoc_input_tty(t_app *app, int *fd, char const *heredoc_end, int exp);
int	heredoc_input_fd(t_app *app, int *fd, char const *heredoc_end, int exp);
static int	heredoc_stop(t_app *app, void *input, int *fd);
static int	err_fatal_heredoc_io(t_app *app);

// From heredoc_get_file_name.c
char const	*get_heredoc_filename(void);

// From heredoc_expand_prompt.c
int	expand_prompt(t_env const *env, char **input);

// From heredoc_open_write.c
int	try_open_heredoc(t_app *app, int /*out*/ *fd);
int	write_heredoc(int *fd, char *input, int const nl);

// From signals.c
void	set_default_signals(void);
void	set_heredoc_signals(void);

//* will close *fd on failure.
int	get_heredoc_input(t_app *app, int *fd, char *heredoc_end, int exp)
{
	int	ret;

	unquote_inplace(heredoc_end);
	if (app->skip_exec)
		return (1); // Skip heredoc input due to prior Ctrl+C.
	if (isatty(STDIN_FILENO))
	{
		set_heredoc_signals();
		ret = heredoc_input_tty(app, fd, heredoc_end, exp);
		set_default_signals();
		if (ret == -1)
			return (app->skip_exec = 1);
		else if (ret == 1)
			return (1);
	}
	else if (!isatty(STDIN_FILENO)
		&& heredoc_input_fd(app, fd, heredoc_end, exp))
		return (1);
	return err_fatal_heredoc_io(app); // Failure!
}

int	heredoc_input_tty(t_app *app, int *fd, char const *heredoc_end, int exp)
{
	char	*input;

	while (1)
	{
		// TODO [MIN-32]: Signals for readline here.
		rl_done = 0;
		input = readline("> ");
		if (!input)
		{
			write(2, "Warning: heredoc delimited by end-of-file (wanted `",
				48);
			// CTRL+D
			// Simply stop. Keep whatever was written to the file `*fd`
			// Discard current `input` prompt.
			return heredoc_stop(app, input, fd);
		}
		else if (rl_done && g_signal == 128 + SIGINT)
		{
			write(2, "wtf\n", 4);
			// Interrupted by SIGINT (Ctrl+C).
			rl_done = 0;
			free(input);
			close(*fd);
			unlink(app->cur_hd_name);
			free(app->cur_hd_name);
			app->cur_hd_name = 0;
			*fd = -1;
			return (-1); // Convey interrupt to caller.
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
	(void)app;
	(void)fd;
	(void)heredoc_end;
	(void)exp;

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
	*fd = app->heredocs[app->cur_hd] = open(app->cur_hd_name,
		O_RDONLY, 0600);
	if (*fd < 0)
		err_fatal_heredoc_io(app);
	unlink(app->cur_hd_name);
	free(app->cur_hd_name);
	app->cur_hd_name = 0;
	free(input);
	return (1);
}

static int	err_fatal_heredoc_io(t_app *app)
{
	write(2, "Fatal: File I/O error during heredocument fd acquisition.\n", 58);
	app_destroy(app);
	exit(1);
}
