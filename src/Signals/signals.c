
#include "minishell.h"

// = Bash convention for SIGINT
void	sigint_handler(int signum)
{
	(void)signum;
	write(1, "\n", 1);
	set_exit_status(130);
}

void	sigquit_handler(int signum)
{
	(void)signum;
}

void	setup_signals(void)
{
	signal(SIGINT, sigint_handler);
	signal(SIGQUIT, sigquit_handler);
}

void	set_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}


// int	main(int argc, char const *argv[], char const *envp[])
// {
// 	setup_signals(); // <==== ADD THIS_Before entering prompt/readline loop
// 	// ...
// }
