/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 19:06:24 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/14 16:03:52 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

/*																			*/
/*	String equal.															*/
/*	Returns 1 if the strings are equal, 0 otherwise.						*/
/*																			*/
int	streq(char const *a, char const *b)
{
	while (*a && *b)
	{
		if (*a != *b)
			return (0);
		a++;
		b++;
	}
	return (*a == *b);
}

void	scpy(char *dst, char const *src)
{
	while (*src)
	{
		*dst = *src;
		src++;
		dst++;
	}
	*dst = 0;
}

unsigned int	get_char_idx(char const *str, char c)
{
	unsigned int	idx;

	idx = ~0;
	while (str[++idx])
	{
		if (str[idx] == c)
			return (idx);
	}
	return (~0);
}

void	*mclone_grow(void const *p, size_t p_size, size_t grow_size)
{
	void	*mem;

	mem = malloc(p_size + grow_size);
	if (mem)
		mcpy(mem, p, p_size);
	return (mem);
}

int	is_wspc(char c)
{
	return (c == ' ' || c == '\r' || c == '\n' || c == '\t' || c == '\v');
}
