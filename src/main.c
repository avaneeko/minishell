/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 18:06:42 by losypenk          #+#    #+#             */
/*   Updated: 2025/08/01 20:36:00 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
		__builtin_printf("%i: %s\r\n", i, e2str[list->tok[i]->type]);
	}
}

int	main(int argc, char const *argv[], char const *envp[])
{
	(void)argc; (void)argv; (void)envp;
	t_token_list *l;
	if (!new_token_list(4096, &l))
		exit(101);
	char *line;
	while ((line = readline("? ")))
	{
		//write(1, line, slen(line));
		if (!tokenize(line, l))
			write(1, "Tokenizer error.", sizeof "Tokenizer error." - 1);
		print_token_list(l);
		clear_token_list(l);
		// write(1, &(char){'\n'}, 1);
		free(line);
	}

	return (0);
}
