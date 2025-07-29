/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token_list.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/29 16:14:56 by losypenk          #+#    #+#             */
/*   Updated: 2025/07/29 17:57:50 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	new_token_list(unsigned int capacity, t_token_list **out)
{
	t_token_list *const list = malloc(sizeof(t_token_list)
		+ capacity * sizeof(t_token *));

	if (!list)
		return (0);
	list->cap = capacity;
	list->len = 0;
	*out = list;
	return (1);
}

void	destroy_token_list(t_token_list const *list)
{
	free((void *)list);
}

void	copy_token_list(t_token_list const *src, t_token_list *dst)
{
	mcpy(dst, src, sizeof(t_token_list) + src->len * sizeof(t_token *));
}

int	clone_token_list(t_token_list const *list, t_token_list **out)
{
	t_token_list *const mem = malloc(sizeof(t_token_list)
		+ list->len * sizeof(t_token *));

	if (mem)
	{
		copy_token_list(list, mem);
		*out = mem;
	}
	return (mem != 0);
}

int	resize_token_list(t_token_list **list, unsigned int new_capacity)
{
	t_token_list	*mem;

	if (new_capacity < (*list)->cap)
		(*list)->len = new_capacity;
	mem = malloc(sizeof(t_token_list)
		+ new_capacity * sizeof(t_token *));
	if (mem)
	{
		copy_token_list(*list, mem);
		mem->cap = new_capacity;
	}
	free(*list);
	*list = mem;
	return (mem != 0);
}
