/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 15:43:44 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/16 15:45:23 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <readline/readline.h>
#include <readline/history.h>
#include <signal.h>
#include <unistd.h>

t_app	*get_app(void);

volatile int	g_signal = 0;

// = Bash convention for SIGINT
static void	sigint_handler_default(int signum)
{
	write(1, "\n", 1);
	rl_on_new_line();
	rl_replace_line("", 0);
	rl_redisplay();
	rl_done = 1;
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

/* Install parent handlers: Ctrl-C interactive behavior, ignore Ctrl-\ .     */
void	set_default_signals(void)
{
	g_signal = 0;
	rl_event_hook = NULL;
	signal(SIGINT, sigint_handler_default);
	signal(SIGQUIT, SIG_IGN);
}

static void	parent_sigint_handler(int signum)
{
	g_signal = 128 + signum;
	get_app()->last_exit_code = g_signal;
}
