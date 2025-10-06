#include "minishell.h"

/**
 * Reset the execution state of the application.
 *
 * Resets the command list in app->exec.
 */

static void	free_redirections(t_redir **redirs)
{
	t_redir	*redir;
	t_redir	*next;

	redir = *redirs;
	while (redir)
	{
		next = redir->next;
		free(redir->target);
		free(redir);
		redir = next;
	}
	*redirs = NULL;
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
}
