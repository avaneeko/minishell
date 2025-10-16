/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_wait.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 20:04:52 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/15 23:56:45 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

/**
 * @brief Convert a waitpid status to a shell exit code and, if last, print the
 * 		signal message.
 * @param wstatus Raw status from waitpid.
 * @param is_last Non-zero to print "Quit (core dumped)" for SIGQUIT and a
 * 		newline for SIGINT.
 * @return WEXITSTATUS if exited, 128 + signal number if signaled, or 1 as a
 * 		generic fallback.
 */
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

/**
 * @brief Wait for all PIDs in the pipeline and compute the final shell status
 * 		from the last process.
 * @param pids Array of child PIDs.
 * @param n_cmd Number of processes to wait for.
 * @return Exit code derived from the last process or signal-based code.
 */
int	wait_pipeline(pid_t *pids, int n_cmd)
{
	int		i;
	int		status;
	int		final;
	pid_t	last;
	pid_t	got;

	if (n_cmd <= 0)
		return (0);
	last = pids[n_cmd - 1];
	final = 0;
	i = 0;
	while (i < n_cmd)
	{
		got = waitpid(-1, &status, 0);
		if (got == last)
			final = translate_wait_status(status, 1);
		else
			(void)translate_wait_status(status, 0);
		i += 1;
	}
	return (final);
}
