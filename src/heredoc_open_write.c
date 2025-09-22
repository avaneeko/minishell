#include "minishell.h"

// This file defines:
int	try_open_heredoc(t_app *app, int /*out*/ *fd);
int	write_heredoc(int *fd, char *input, int const nl);
static int	write_heredoc_failure(int *fd, void *input);
static void	err_heredoc_io(t_app *app);

// From heredoc_get_file_name.c
char	*get_heredoc_filename(void);

// Try to open a new heredoc. Opens a file to be used for heredoc.
// fd - heredoc fd.
int	try_open_heredoc(t_app *app, int /*out*/ *fd)
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
	app->cur_hd_name = get_heredoc_filename();
	if (!app->cur_hd_name)
		err_heredoc_io(app);
	app->heredocs[i] = open(app->cur_hd_name, O_RDWR | O_CREAT | O_EXCL,
		0600);
	app->cur_hd = i;
	if (app->heredocs[i] == -1)
		err_heredoc_io(app);
	*fd = app->heredocs[i];
	// can't unlink here cuz the file needs to be re-opened later.
	return (1);
}

static void	err_heredoc_io(t_app *app)
{
	write(2, "minishell: I/O failure during heredoc opening.\n", 47);
	app_destroy(app);
	exit(1);
}

static int	write_heredoc_failure(int *fd, void *input)
{
	write(2, "minishell: Failure occurred when writing to a heredoc"
		" file descriptor. Aborting prompt.\n", 88);
	close(*fd);
	free(input);
	return ((*fd = -1) == 0);
}

// Util to write heredoc input and free it with error reporting.
// Always frees `input`
// Closes `*fd` in case of failure.
int	write_heredoc(int *fd, char *input, int const nl)
{
	if (nl)
	{
		if (!write_exact(*fd, input, slen(input)) || !write_exact(*fd, "\n", 1))
		{
			return (write_heredoc_failure(fd, input));
		}
	}
	else if (!write_exact(*fd, input, slen(input)))
	{
		return (write_heredoc_failure(fd, input));
	}
	free(input);
	return (1);
}
