/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line.h                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/02 10:55:12 by losypenk          #+#    #+#             */
/*   Updated: 2025/05/02 11:09:10 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef GET_NEXT_LINE_H
# define GET_NEXT_LINE_H
# ifndef BUFFER_SIZE
#  define BUFFER_SIZE 4096
# endif

# include <unistd.h>
# include <stdlib.h>

typedef struct s_buffer
{
	int		state;
	size_t	idx;
	size_t	size;
	char	buf[BUFFER_SIZE];
}	t_buffer;

typedef struct s_line_reader
{
	char	*chain;
	size_t	chain_len;
	char	*link;
	size_t	link_len;
	int		nl;
}	t_line_reader;

char	*get_next_line(int fd);

ssize_t	find_nl(void const *mem, size_t len);
int		strchain(char **chain, char const *link, size_t chain_len,
			size_t link_len);

#endif
