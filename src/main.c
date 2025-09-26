#include "minishell.h"

int g_exit_status = 0;

void	print_token_list(t_token_list *list)
{
	char const *e2str[] = {
		"TOKEN_UNDEFINED",
		"TOKEN_WORD",
		"TOKEN_PIPE",
		"TOKEN_REDIRECT_INPUT",
		"TOKEN_REDIRECT_OUTPUT",
		"TOKEN_REDIRECT_OUTPUT_APPEND",
		"TOKEN_HEREDOC",
		"TOKEN_VARIABLE"
	};
	for (unsigned int i = 0; i < list->len; i++)
	{
		if ((list->tok[i]->type ^ TOKEN_HEREDOC) == 0)
			__builtin_printf("%i: %s #%u\r\n", i, e2str[list->tok[i]->type], (int unsigned)list->tok[i]->token[0]);
		else
			__builtin_printf("%i: %s %s\r\n", i, e2str[list->tok[i]->type], list->tok[i]->token);
	}
}

// int	main(int argc, char const *argv[], char const *envp[])
// {
// 	t_app app;

// 	if (!app_create(argc, argv, envp, &app))
// 		return EXIT_FAILURE;

// 	for	(unsigned int i = 0; i < app.env.len; ++i)
// 	{
// 		__builtin_dump_struct(&app.env.pairs[i], &printf);
// 	}

// 	app_destroy(&app);
// 	return EXIT_SUCCESS;
// }

/*
	Debug only; do not ship.
	$ Prints all of the heredocuments contents along with their id's.
*/
static void Debug_PrintAllHeredocumentContents( t_app * app )
{
	void * buf = __builtin_alloca(4096);

	for ( int unsigned i = 0; i < 16; i++ )
	{
		if ( app->heredocs[i] != -1 )
		{
			__builtin_printf("Heredoc #%u:\r\n", i);
			ssize_t bytesRead;
			while ( (bytesRead = read(app->heredocs[i], buf, 4096)) > 0 )
			{
				write(STDOUT_FILENO, buf, bytesRead);
			}
			lseek(app->heredocs[i], 0, SEEK_SET);
		}
	}
}

int	main(int argc, char const *argv[], char const *envp[])
{
	t_app app;

	setup_signals();
	if (!app_create(argc, argv, envp, &app))
		return EXIT_FAILURE;
	char *line;
	while ((line = readline("$ ")))
	{
		if (!tokenize(line, app.token_list))
		{
			write(1, "Tokenizer error.", sizeof "Tokenizer error." - 1);
			free(line);
			continue;
		}
		if (!prompt_heredoc(&app))
			break;
		expand(&app.token_list, &app.env);
		token_resplit(&app);
		print_token_list(app.token_list);
		build_exec(&app);
		execute_pipeline(&app.exec.cmds[0], &app.env);
		clear_token_list(app.token_list);
		Debug_PrintAllHeredocumentContents(&app);
		app_reset_heredocs(&app);
		// write(1, &(char){'\n'}, 1);
		free(line);
	}

	return (0);
}
