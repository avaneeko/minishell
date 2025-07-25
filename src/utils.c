/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 18:51:42 by losypenk          #+#    #+#             */
/*   Updated: 2025/07/25 19:02:00 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

/*																			*/
/*	Memory copy.															*/
/*																			*/
void	mcpy(void *dst, void const *src, size_t size)
{
	size_t	i;

	i = 0;
	while (i < size)
	{
		((unsigned char *)dst)[i] = ((unsigned char const *)src)[i];
		i++;
	}
}

/*																			*/
/*	Memory move.															*/
/*																			*/
void	mmov(void *src, void *dst, size_t size)
{
	unsigned char *const	s = src;
	unsigned char *const	d = dst;
	size_t					i;

	i = 0;
	if (src < dst)
	{
		while (i < size)
		{
			d[size - i - 1] = s[size - i - 1];
			i++;
		}
	}
	else if (dst < src)
	{
		while (i < size)
		{
			d[i] = s[i];
			i++;
		}
	}
}

/*																			*/
/*	Memory compare.															*/
/*																			*/
int	mcmp(void const *s1, void const *s2, size_t size)
{
	size_t	i;

	i = 0;
	while (i < size)
	{
		if (((unsigned char const *)s1)[i] != ((unsigned char const *)s2)[i])
			return (((unsigned char const *)s1)[i] -
				((unsigned char const *)s2)[i]);
		i++;
	}
	return (0);
}

/*																			*/
/*	String length.															*/
/*																			*/
size_t	slen(char const *s)
{
	size_t	i;

	i = 0;
	while (s[i])
	{
		i++;
	}
	return (i);
}

/*																			*/
/*	Memory clone															*/
/*																			*/
void	*mclone(void const *p, size_t size)
{
	void	*mem;

	mem = malloc(size);
	if (mem)
		mcpy(mem, p, size);
	return (mem);
}
