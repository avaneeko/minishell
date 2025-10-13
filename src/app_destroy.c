/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   app_destroy.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 21:04:05 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/13 21:04:34 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	close_heredocs(t_app *app)
{
	int unsigned	i;

	i = ~0;
	while (++i < 16)
	{
		if (app->heredocs[i] != -1)
			close(app->heredocs[i]);
	}
	mset(app->heredocs, -1, sizeof(app->heredocs));
	if (app->cur_hd_name)
		unlink(app->cur_hd_name);
	free(app->cur_hd_name);
	app->cur_hd_name = 0;
}

void	app_reset_heredocs(t_app *app)
{
	int unsigned	i;

	i = ~0;
	while (++i < 16)
	{
		if (app->heredocs[i] != -1)
		{
			close(app->heredocs[i]);
			app->heredocs[i] = -1;
		}
	}
	if (app->cur_hd_name)
		unlink(app->cur_hd_name);
	free(app->cur_hd_name);
	app->cur_hd_name = 0;
}

void	app_destroy(t_app *app)
{
	destroy_env(&app->env);
	destroy_token_list_deep(app->token_list);
	close_heredocs(app);
	if (app->prompt)
		free(app->prompt);
	app->prompt = 0;
	cmdarr_destroy(&app->exec);
	free(app->pids);
	app->pids = 0;
}
