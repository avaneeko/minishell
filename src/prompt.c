#include "minishell.h"

static int	prompt_tty(t_app *app)
{
	rl_done = 0;
	app->prompt = readline("minishell$ ");
	if (!app->prompt)
		return (0); // EOF (Ctrl-D)
	if (slen(app->prompt) > 0)
		add_history(app->prompt);
	return (1);
}

// This function is only implemented for the tester,
// we might not need to ship it.
static int	prompt_fd(t_app *app)
{
	//! So this function is simply broken. Please do not use this.
	__builtin_debugtrap(/* BROKEN FUNCTION */);
	char	*buffer;
	char	*new_buffer;
	ssize_t	bytes_read;
	size_t	total_size;
	size_t	buffer_size;

	// Initialize variables
	buffer = malloc(1024);
	if (!buffer)
		return (0);
	buffer_size = 1024;
	total_size = 0;

	// Read all data from stdin until EOF
	while ((bytes_read = read(STDIN_FILENO, buffer + total_size,
	                         buffer_size - total_size - 1)) > 0)
	{
		total_size += bytes_read;

		// Resize buffer if needed
		if (total_size >= buffer_size - 1)
		{
			buffer_size *= 2;
			new_buffer = realloc(buffer, buffer_size);
			if (!new_buffer)
			{
				free(buffer);
				return (-1);
			}
			buffer = new_buffer;
		}
	}

	if (bytes_read == 0)
	{
		free(buffer);
		return (0); // EOF
	}

	if (bytes_read == 0)
	{
		free(buffer);
		return (0); // EOF
	}

	// Handle read error
	if (bytes_read == -1)
	{
		free(buffer);
		return (-1);
	}

	// Null-terminate the string
	buffer[total_size] = '\0';

	// Remove trailing newline if present (to match readline behavior)
	if (total_size > 0 && buffer[total_size - 1] == '\n')
		buffer[total_size - 1] = '\0';

	app->prompt = buffer;
	return (1);
}

int	prompt(t_app *app)
{
	int (*const dispatch[2])(t_app *app) = {prompt_fd, prompt_tty};

	return dispatch[1](app);
	return dispatch[isatty(STDOUT_FILENO)](app);
}

// #include "minishell.h"
// #include "get_next_line.h"

// static int	prompt_tty(t_app *app)
// {
// 	rl_done = 0;
// 	app->prompt = readline("minishell$ ");
// 	if (!app->prompt)
// 		return (0); // EOF (Ctrl-D)
// 	if (slen(app->prompt) > 0)
// 		add_history(app->prompt);
// 	return (1);
// }

// static int	prompt_fd(t_app *app)
// {
// 	app->prompt = get_next_line(STDIN_FILENO);
// 	if (!app->prompt)
// 		return (0); // EOF
// 	return (1);
// }

// int	prompt(t_app *app)
// {
// 	int (*const dispatch[2])(t_app *app) = {prompt_fd, prompt_tty};

// 	return dispatch[isatty(STDIN_FILENO)](app);
// }
