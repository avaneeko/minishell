#include "minishell.h"
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <signal.h>

/*
* Close both ends of a pipe if they are valid.
*/
void	close_pipe(int pipefd[2])
{
	if (pipefd[0] >= 0)
	{
		close(pipefd[0]);
	}
	if (pipefd[1] >= 0)
	{
		close(pipefd[1]);
	}
}

/*
* Duplicate fd src into dest, close src if different.
* Return 0 on success, -1 on error.
*/
int	safe_dup2(int src, int dest)
{
	if (src == dest)
	{
		return (0);
	}
	if (dup2(src, dest) < 0)
	{
		return (-1);
	}
	close(src);
	return (0);
}

/*
* Restore std fds from saved values, then close the saved fds.
*/
void	restore_std_fds(int saved[2])
{
	dup2(saved[0], STDIN_FILENO);
	dup2(saved[1], STDOUT_FILENO);
	close(saved[0]);
	close(saved[1]);
}

/*
* Save copies of stdin and stdout for restoration.
* Returns 0 on success, -1 on error.
*/
int	save_std_fds(int saved[2])
{
	saved[0] = dup(STDIN_FILENO);
	saved[1] = dup(STDOUT_FILENO);
	if (saved[0] == -1 || saved[1] == -1)
	{
		return (-1);
	}
	return (0);
}
