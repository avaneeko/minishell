/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   prompt.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:05:30 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/13 14:17:48 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "get_next_line.h"

static int	prompt_tty(t_app *app)
{
	rl_done = 0;
	app->prompt = readline("minishell$ ");
	if (!app->prompt)
		return (0);
	if (slen(app->prompt) > 0)
		add_history(app->prompt);
	return (1);
}

static int	prompt_fd(t_app *app)
{
	app->prompt = get_next_line(STDIN_FILENO);
	if (!app->prompt)
		return (0);
	return (1);
}

// Remove `1 & ` to use get_next_line instead of
// readline when reading from a non-tty STDIN.
// Some testers require that, while others fail.
//
// Removed:
//  int (*const dispatch[2])(t_app *) = {prompt_fd, prompt_tty};
// Since the norm doesn't recognize that as a constant even though it is.
// So we just use a simple conditional instead.
int	prompt(t_app *app)
{
	int	(*dispatch[2])(t_app *app);

	dispatch[0] = prompt_fd;
	dispatch[1] = prompt_tty;
	return (dispatch[1 & isatty(STDIN_FILENO)](app));
}
