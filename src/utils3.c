/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 16:05:19 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/14 16:05:20 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	mset(void *dst, int c, size_t count)
{
	size_t	i;

	i = ~0;
	while (++i < count)
	{
		*((char unsigned *)dst + i) = (char unsigned)c;
	}
}

int	write_exact(int fd, void const *p, size_t size)
{
	return (write(fd, p, size) == (ssize_t)size);
}

void	unquote_inplace(char *s)
{
	char	*w;

	w = s;
	while (*s)
	{
		if (*s != '\'' && *s != '"')
			*w++ = *s;
		s++;
	}
	*w = 0;
}
