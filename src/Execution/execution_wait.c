/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_wait.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 20:04:52 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/14 17:05:21 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

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
