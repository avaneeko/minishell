/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_pwd.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:20:35 by jgueon            #+#    #+#             */
/*   Updated: 2025/09/16 18:43:10 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"
#include <unistd.h>     // getcwd
#include <linux/limits.h>   // PATH_MAX
#include <stdlib.h>         //perror

// Print the current working directory
int builtin_pwd(void)
{
    char cwd[PATH_MAX];

    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        perror("pwd");
        return (1);
    }
    write(1, cwd, slen(cwd));
    write(1, "\n", 1);
    return (0);
}
