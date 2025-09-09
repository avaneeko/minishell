/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_echo.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:14:04 by jgueon            #+#    #+#             */
/*   Updated: 2025/09/09 20:39:20 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

// Prints its arguments separated by spaces, handles '-n' (no newline)
// Returns 0 always
int builtin_echo(char **argv)
{
    int i;
    int newline;

    i = 1;
    newline = 1;
    if (argv[1] && streq(argv[1], "-n"))
    {
        newline = 0;
        i++;
    }
    while (argv[i])
    {
        write(1, argv[i], slen(argv[i]));
        if (argv[i + 1])
            write(1, " ", 1);
        i++;
    }
    if (newline)
        write(1, "\n", 1);
    return (0);
}
