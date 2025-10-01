
/* ************************************************************************** */
/*                                                                            */
/*                                signals.c                                    */
/*                                                                            */
/*   Overview                                                                  */
/*   - Parent (interactive shell):                                             */
/*       * SIGINT prints a newline, clears input, redisplays prompt,           */
/*         and sets exit status 130 (bash-like at the prompt).                 */
/*       * SIGQUIT is ignored in interactive mode.                             */
/*   - Children: restore SIGINT/SIGQUIT defaults so executed programs behave   */
/*     normally (call set_child_signals() after fork, before exec).            */
/*                                                                            */
/*   Integration                                                               */
/*   - Call setup_signals() once before the readline loop in main.            */
/*   - Call set_child_signals() in the child code path right after fork.       */
/*   - For heredoc in a dedicated child, call set_heredoc_signals().           */
/*                                                                            */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <unistd.h>

/* SIGINT at prompt: newline, clear current input, redisplay, status=130. */
// = Bash convention for SIGINT
void	sigint_handler(int signum)
{
	(void)signum;
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	// get_app()->last_exit_status = 130;
}

/* Ignore SIGQUIT in interactive parent: do nothing and do not change $?. */
/* Ignore SIGQUIT in interactive parent. */
/* intentionally empty to ignore Control-\ at the prompt */
void	sigquit_handler(int signum)
{
	(void)signum;
	/* Intentionally empty to mirror bash in interactive mode.  */
}

/* Install interactive handlers in the parent shell (before readline loop). */
/* Install parent handlers: Ctrl-C interactive behavior, ignore Ctrl-\ .     */
void	setup_signals(void)
{
	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, sigquit_handler);
}

/* In children: restore defaults so external programs see real signals. */
void	set_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}



/* Optional: in a heredoc child, abort on Ctrl-C but ignore Ctrl-\ like bash. */
void    set_heredoc_signals(void)
{
    signal(SIGINT, SIG_DFL);
    signal(SIGQUIT, SIG_IGN);
}

/* Optional: restore interactive handlers again (wrapper for clarity). */
void    restore_interactive_signals(void)
{
    setup_signals();
}

// int	main(int argc, char const *argv[], char const *envp[])
// {
// 	setup_signals(); // <==== ADD THIS_Before entering prompt/readline loop
// 	// ...
// }
