/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/24 18:06:42 by losypenk          #+#    #+#             */
/*   Updated: 2025/07/25 19:34:53 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	main(int argc, char const *argv[], char const *envp[])
{
	char *line;
	while ((line = readline("? ")))
	{
		write(2, line, slen(line));
		free(line);
	}

	return (0);
}
