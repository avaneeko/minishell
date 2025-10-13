/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc_input1.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 16:22:17 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/13 16:23:06 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Called by heredoc_input_tty() when terminated by SIGINT.
int	on_input_tty_term_sigint(t_app *app, void *input, int *fd)
{
	rl_done = 0;
	free(input);
	close(*fd);
	unlink(app->cur_hd_name);
	free(app->cur_hd_name);
	app->cur_hd_name = 0;
	return (*fd = -1);
}
