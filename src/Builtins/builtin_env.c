/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_env.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 17:26:53 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/09 17:29:31 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

int	builtin_env(t_env *env)
{
	unsigned int	i;

	if (!env)
		return (0);
	i = 0;
	while (i < env->len)
	{
		if (env->pairs[i].key && env->pairs[i].value)
		{
			write(1, env->pairs[i].key, slen(env->pairs[i].key));
			write(1, "=", 1);
			write(1, env->pairs[i].value, slen(env->pairs[i].value));
			write(1, "\n", 1);
		}
		i++;
	}
	return (0);
}
