#include "execution_utils.h"
#include "../Builtins/builtins_utils.h"
#include "minishell.h"
#include <unistd.h> /* close */
#include <stdlib.h> /* free */
#include <errno.h>       /* EINTR */
#include <sys/wait.h>
#include <signal.h>   /* waitpid, WIF* macros */


void close_and_free_pipes(int n_cmd, int **pipes)
{
    int i;

    if (!pipes)
        return;
    i = 0;
    while (i < n_cmd - 1)
    {
        close(*pipes[i]); /* read end */
        close(pipes[i][2]); /* write end */
        free(pipes[i]);
        i++;
    }
    free(pipes);
}

/* Free a NULL-terminated environment array created by env_serialize. */
void env_free_serialized(char **envp)
{
    int i;

    if (!envp)
        return;
    i = 0;
    while (envp[i])
    {
        free(envp[i]);
        i++;
    }
    free(envp);
}


/* ************************************************************************** */
/*                                                                            */
/*                          execution_wait.c                                   */
/*                                                                            */
/*  Purpose:                                                                  */
/*  - Wait for all children in a pipeline.                                    */
/*  - Return the shell status of the last command in the pipeline.            */
/*  - Free the pids array.                                                     */
/*                                                                            */
/*  Behavior details (bash-like):                                             */
/*  - If the last process exits normally, return its WEXITSTATUS.             */
/*  - If it is terminated by a signal, return 128 + signal.                   */
/*  - While waiting, if the last died by SIGINT, print a newline.             */
/*  - If the last died by SIGQUIT, print "Quit (core dumped)".                */
/*                                                                            */
/*  Notes:                                                                    */
/*  - No for-loops, no ternary; <=25 lines per function body (Norm V3).      */
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

/* Wait all PIDs; return the last command's status; free the pids array.      */
int	wait_pipeline(pid_t *pids, int n_cmd)
{
	int		i;
	int		status;
	int		code;
	int		last_code;
	pid_t	got;
	pid_t	last;

	i = 0;
	last = pids[n_cmd - 1];
	last_code = 0;
	while (i < n_cmd)
	{
		got = waitpid(-1, &status, 0);
		if (got == -1)
		{
			if (errno == EINTR)
				continue;
			break;
		}
		code = translate_wait_status(status, got == last);
		if (got == last)
			last_code = code;
		i++;
	}
	free(pids);
	return (last_code);
}
