/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_list2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 22:03:21 by losypenk          #+#    #+#             */
/*   Updated: 2025/07/31 16:46:42 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	append_token_list(t_token_list **list, t_token *token)
{
	if ((*list)->len + 1 > (*list)->cap)
	{
		if (!resize_token_list(list, (*list)->cap + 4096))
		{
			destroy_token(token);
			return (0);
		}
	}
	(*list)->tok[(*list)->len++] = token;
	return (1);
}

void	destroy_token_list_deep(t_token_list const *list)
{
	unsigned int	i;

	i = 0;
	while (i < list->len)
		destroy_token(list->tok[i++]);
	destroy_token_list(list);
}
