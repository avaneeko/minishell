/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app_reset_exec.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 17:09:57 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/14 17:10:06 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/**
 * Reset the execution state of the application.
 *
 * Resets the command list in app->exec.
 */

/**
 * Free the redirections array, remember that redirs[i]->target is not owned
 * by the t_redir, so it is not freed here.
 */
static void	free_redirections(t_redir **redirs)
{
	int unsigned	i;

	if (!redirs)
		return ;
	i = ~0;
	while (redirs[++i])
	{
		free(redirs[i]);
	}
	free(redirs);
}

void	app_reset_exec(t_app *app)
{
	int unsigned	i;

	i = ~0;
	while (++i < app->exec.len)
	{
		free(app->exec.cmds[i].argv);
		app->exec.cmds[i].argv = NULL;
		if (app->exec.cmds[i].redirs)
			free_redirections(app->exec.cmds[i].redirs);
	}
	app->exec.len = 0;
}
