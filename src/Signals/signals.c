#include "minishell.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <unistd.h>

t_app *get_app(void);

volatile int g_signal = 0;

// = Bash convention for SIGINT
static void	sigint_handler_default(int signum)
{
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	g_signal = 128 + signum;
	get_app()->last_exit_code = g_signal;
}

static int	heredoc_event_hook(void)
{
	if (g_signal == 130)
	{
		rl_done = 1;
		return (1);
	}
	return (0);
}

static void	sigint_handler_heredoc(int signum)
{
	rl_done = 1;
	g_signal = 128 + signum;
	get_app()->last_exit_code = g_signal;
}

/* Ignore SIGQUIT in interactive parent: do nothing and do not change $?. */
static void	sigquit_handler(int signum)
{
	(void)signum;
	/* Intentionally empty to mirror bash in interactive mode.  */
}

/* Install parent handlers: Ctrl-C interactive behavior, ignore Ctrl-\ .     */
void	set_default_signals(void)
{
	g_signal = 0;
	rl_event_hook = NULL;
	signal(SIGINT, sigint_handler_default);
	signal(SIGQUIT, SIG_IGN);
}

void	set_heredoc_signals(void)
{
	g_signal = 0;
	rl_event_hook = heredoc_event_hook;
	signal(SIGINT, sigint_handler_heredoc);
	signal(SIGQUIT, SIG_IGN);
}

void	set_child_signals(void)
{
	// g_signal = 0;
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}


// int	main(int argc, char const *argv[], char const *envp[])
// {
// 	setup_signals(); // <==== ADD THIS_Before entering prompt/readline loop
// 	// ...
// }
