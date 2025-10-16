/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   signals1.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/16 15:44:03 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/16 15:51:23 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include <signal.h>

// From signals.c
void	parent_sigint_handler(int signum);
void	sigint_handler_heredoc(int signum);
int		heredoc_event_hook(void);

void	set_parent_signals(void)
{
	signal(SIGINT, parent_sigint_handler);
	signal(SIGQUIT, SIG_DFL);
}

void	set_child_signals(void)
{
	signal(SIGINT, SIG_DFL);
	signal(SIGQUIT, SIG_DFL);
}

void	set_heredoc_signals(void)
{
	g_signal = 0;
	rl_event_hook = heredoc_event_hook;
	signal(SIGINT, sigint_handler_heredoc);
	signal(SIGQUIT, SIG_IGN);
}
