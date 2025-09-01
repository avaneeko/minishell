#include "minishell.h"

int	tokenize(char *str, t_token_list *list);

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

//! HOTWIRE
int	expand(t_token_list **list, t_env const *env);

int	main(int argc, char const *argv[], char const *envp[])
{
	t_app app;
	if (!app_create(argc, argv, envp, &app))
		return EXIT_FAILURE;
	char *line;
	while ((line = readline("$ ")))
	{
		//write(1, line, slen(line));
		if (!tokenize(line, app.token_list))
			write(1, "Tokenizer error.", sizeof "Tokenizer error." - 1);
		expand(&app.token_list, &app.env);
		// print_token_list(app.token_list);
		clear_token_list(app.token_list);
		// write(1, &(char){'\n'}, 1);
		free(line);
	}

	return (0);
}
