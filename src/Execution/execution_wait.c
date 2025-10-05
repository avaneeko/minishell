/* ************************************************************************** */
/*					execution_wait.c										  */
/*  - While waiting, if the last died by SIGINT, print a newline.             */
/*  - If the last died by SIGQUIT, print "Quit (core dumped)".                */
/*                                                                            */
/*  Notes:                                                                    */
/*  - Parent keeps custom handlers; children use SIG_DFL (see set_child_signals). */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <sys/wait.h>    /* waitpid, WIF* macros */
#include <signal.h>      /* SIGINT, SIGQUIT */
#include <unistd.h>      /* write */
#include <errno.h>       /* EINTR */
#include <stdlib.h>      /* free */

/* Translate a wait status to a shell exit code; print once for last proc.    */
static int	translate_wait_status(int wstatus, int is_last)
{
	int	sig;

	if (WIFEXITED(wstatus))
		return (WEXITSTATUS(wstatus));
	if (WIFSIGNALED(wstatus))
	{
		sig = WTERMSIG(wstatus);
		if (is_last)
		{
			if (sig == SIGQUIT)
				write(2, "Quit (core dumped)\n", 19);
			if (sig == SIGINT)
				write(2, "\n", 1);
		}
		return (128 + sig);
	}
	return (1);
}

// //* Wait each pid directly; return status of the last pid in the pipeline. */
// int	wait_pipeline(pid_t *pids, int n_cmd)
// {
// 	int		i;
// 	int		status;
// 	int		last_code;

// 	(void)pids;
// 	i = 0;
// 	last_code = 0;
// 	while (i < n_cmd)
// 	{
// 		// if (waitpid(pids[i], &status, 0) > 0)
// 		if (waitpid(-1, &status, 0))
// 		{
// 			if (i == n_cmd - 1)
// 				last_code = translate_wait_status(status, 1);
// 		}
// 		i += 1;
// 	}
// 	return (last_code);
// }

int wait_pipeline(pid_t *pids, int n_cmd)
{
    int   waited;
    int   status;
    int   last_code;
    pid_t last;
    pid_t pid;

    waited = 0;
    last_code = 0;
    last = pids[n_cmd - 1];
    while (waited < n_cmd)
    {
        pid = waitpid(-1, &status, 0);
        if (pid <= 0)
            break; /* error or unexpected */
        if (pid == last)
            last_code = translate_wait_status(status, 1);
        waited += 1;
    }
    return (last_code);
}
