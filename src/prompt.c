#include "minishell.h"
#include "get_next_line.h"

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

static int	prompt_fd(t_app *app)
{
	app->prompt = get_next_line(STDIN_FILENO);
	if (!app->prompt)
		return (0); // EOF
	return (1);
}

int	prompt(t_app *app)
{
	int (*const dispatch[2])(t_app *app) = {prompt_fd, prompt_tty};

	return dispatch[isatty(STDIN_FILENO)](app);
}
