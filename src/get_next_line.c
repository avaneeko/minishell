/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/24 13:45:39 by losypenk          #+#    #+#             */
/*   Updated: 2025/05/02 11:06:12 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

void	buffer_saturate(t_buffer *b, int fd)
{
	ssize_t	bytes_read;

	if (b->state == 0 || b->state == -1
		|| (b->state == 1 && b->idx >= b->size))
	{
		bytes_read = read(fd, b->buf, BUFFER_SIZE);
		if (bytes_read == -1 || bytes_read == 0)
		{
			b->state = -1;
			return ;
		}
		b->size = (size_t)bytes_read;
		b->idx = 0;
		b->state = 1;
	}
}

int	buffer_read(int fd, char **str, size_t *len, int *nl_found)
{
	static t_buffer	b;
	ssize_t			pos;

	buffer_saturate(&b, fd);
	if (b.state == -1)
		return (0);
	pos = find_nl(b.buf + b.idx, b.size - b.idx);
	if (pos != -1)
	{
		*str = b.buf + b.idx;
		*len = (size_t)pos + 1;
		*nl_found = 1;
		b.idx += (size_t)pos + 1;
	}
	else
	{
		*str = b.buf + b.idx;
		*len = b.size - b.idx;
		*nl_found = 0;
		b.idx += b.size - b.idx;
	}
	return (1);
}

char	*get_next_line(int fd)
{
	t_line_reader	state;

	state.chain = 0;
	state.chain_len = 0;
	state.link = 0;
	state.link_len = 0;
	state.nl = 0;
	while (state.nl != 1)
	{
		if (!buffer_read(fd, &state.link, &state.link_len, &state.nl))
			break ;
		if (!strchain(&state.chain, state.link, state.chain_len,
				state.link_len))
			return (0);
		state.chain_len += state.link_len;
	}
	return (state.chain);
}
