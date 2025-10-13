/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_input.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 16:04:42 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/13 16:25:29 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "get_next_line.h"

// This file defines:
int			get_heredoc_input(t_app *app, int *fd, char *heredoc_end, int exp);
int			heredoc_input_tty(t_app *app, int *fd, char const *heredoc_end,
				int exp);
int			heredoc_input_fd(t_app *app, int *fd, char const *heredoc_end,
				int exp);
static int	heredoc_stop(t_app *app, void *input, int *fd,
				int const print_heredoc_delim_msg);
static int	err_fatal_heredoc_io(t_app *app);

// From heredoc_input1.c
int			on_input_tty_term_sigint(t_app *app, void *input, int *fd);

// From heredoc_get_file_name.c
char const	*get_heredoc_filename(void);

// From heredoc_expand_prompt.c
int			expand_prompt(t_env const *env, char **input);

// From heredoc_open_write.c
int			try_open_heredoc(t_app *app, int *fd);
int			write_heredoc(int *fd, char *input, int const nl);

// From signals.c
void		set_default_signals(void);
void		set_heredoc_signals(void);

//* will close *fd on failure.
int	get_heredoc_input(t_app *app, int *fd, char *heredoc_end, int exp)
{
	int	ret;

	unquote_inplace(heredoc_end);
	if (app->skip_exec)
		return (1);
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
	return (err_fatal_heredoc_io(app));
}

int	heredoc_input_tty(t_app *app, int *fd, char const *heredoc_end, int exp)
{
	char	*input;

	while (1)
	{
		rl_done = 0;
		input = readline("> ");
		if (!input)
			return (heredoc_stop(app, input, fd, -1));
		else if (rl_done && g_signal == 128 + SIGINT)
			return (on_input_tty_term_sigint(app, input, fd));
		else if (streq(input, heredoc_end))
			return (heredoc_stop(app, input, fd, 0));
		else if (input && exp)
		{
			if (!expand_prompt(&app->env, &input)
				|| !write_heredoc(fd, input, 1))
				return (0);
		}
		else if (!write_heredoc(fd, input, 1))
			return (0);
	}
	return (1);
}

int	heredoc_input_fd(t_app *app, int *fd, char const *heredoc_end, int exp)
{
	char	*input;

	while (1)
	{
		input = get_next_line(STDIN_FILENO);
		if (!input)
			return (heredoc_stop(app, input, fd, -1));
		else if (streq(input, heredoc_end))
			return (heredoc_stop(app, input, fd, 0));
		else if (input && exp)
		{
			if (!expand_prompt(&app->env, &input)
				|| !write_heredoc(fd, input, 1))
				return (0);
		}
		else
		{
			if (!write_heredoc(fd, input, 1))
				return (0);
		}
	}
	return (1);
}

// Reopens current heredoc and frees the `input` and returns 1.
static int	heredoc_stop(t_app *app, void *input, int *fd,
	int const print_heredoc_delim_msg)
{
	if (print_heredoc_delim_msg)
		write(2, "Warning: heredoc delimited by end-of-file\n", 41);
	close(app->heredocs[app->cur_hd]);
	*fd = open(app->cur_hd_name,
			O_RDONLY, 0600);
	app->heredocs[app->cur_hd] = *fd;
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
