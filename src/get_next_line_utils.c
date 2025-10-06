/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 14:48:12 by losypenk          #+#    #+#             */
/*   Updated: 2025/05/02 12:24:23 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

ssize_t	find_nl(void const *mem, size_t len)
{
	size_t	idx;

	idx = 0;
	while (idx < len)
	{
		if (((char *)mem)[idx] == '\n')
			return (idx);
		idx++;
	}
	return (-1);
}

static void	cpy(void *dst, void const *src, size_t size)
{
	size_t	i;

	i = 0;
	while (i < size)
	{
		*((char *)dst + i) = *((char const *)src + i);
		i++;
	}
}

int	strchain(char **chain, char const *link, size_t chain_len,
		size_t link_len)
{
	char	*new;

	if (*chain == 0)
	{
		new = malloc(link_len + 1);
		if (!new)
			return (0);
		cpy(new, link, link_len);
		new[link_len] = 0;
		*chain = new;
		return (1);
	}
	new = malloc(chain_len + link_len + 1);
	if (!new)
	{
		free(*chain);
		return (0);
	}
	cpy(new, *chain, chain_len);
	cpy(new + chain_len, link, link_len);
	new[chain_len + link_len] = 0;
	free(*chain);
	*chain = new;
	return (1);
}
