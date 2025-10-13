/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   env1.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 14:26:17 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/13 14:26:42 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	remove_epair_by_key(t_env *env, char const *key)
{
	unsigned int	i;

	i = ~0u;
	while (++i < env->len)
	{
		if (streq(env->pairs[i].key, key))
		{
			remove_epair_at_idx(env, i);
			return (1);
		}
	}
	return (0);
}
