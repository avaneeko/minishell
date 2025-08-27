/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cd.c                                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:01:30 by jgueon            #+#    #+#             */
/*   Updated: 2025/08/23 19:10:32 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

// Changes the current directory to argv[1] (or $HOME if no arg)
// Returns 0 (success) or 1 (failure)
int builtin_cd(char **argv, t_env *env)
{
    char *home;

    if (!argv[1])
    {
        home = getenv("HOME");
        if (!home)
        {
            write(2, "cd: HOME not set\n", 17);
            return (1);
        }
        if (chdir(home) != 0)
        {
            perror("cd");
            return (1);
        }
        return (0);
    }
    if (chdir(argv[1]) != 0)
    {
        perror("cd");
        return (1);
    }
    (void)env;
    return (0);
}

