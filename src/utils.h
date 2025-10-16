/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 16:10:17 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/14 16:11:07 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_H
# define UTILS_H

# include <stdint.h>
# include <stdlib.h>

//*$	This file defines fundamental, commonly used functions.

/*																			*/
/*	Memory copy.															*/
/*																			*/
void			mcpy(void *dst, void const *src, size_t size);

/*																			*/
/*	Memory move.															*/
/*																			*/
void			mmov(void *src, void *dst, size_t size);

/*																			*/
/*	Memory compare.															*/
/*																			*/
int				mcmp(void const *s1, void const *s2, size_t size);

/*																			*/
/*	String length.															*/
/*																			*/
size_t			slen(char const *s);

/*																			*/
/*	Memory clone															*/
/*																			*/
void			*mclone(void const *p, size_t size);

/*																			*/
/*	String equal.															*/
/*	Returns 1 if the strings are equal, 0 otherwise.						*/
/*																			*/
int				streq(char const *a, char const *b);

/*																			*/
/*	String copy.															*/
/*																			*/
void			scpy(char *dst, char const *src);

/*																			*/
/*	Memory set.																*/
/*																			*/
void			mset(void *dst, int c, size_t count);

/*																			*/
/*	Gets the index of the first character's `c' occurrence in string `str`.	*/
/*	Returns index or ~0 (-1) if `c` is not present in `str`.				*/
/*																			*/
unsigned int	get_char_idx(char const *str, char c);

/*																			*/
/*	Memory clone with growth.												*/
/*	Like mclone() but allows to grow the clone's allocation.				*/
/*																			*/
void			*mclone_grow(void const *p, size_t p_size, size_t grow_size);

/*																			*/
/*	Is char a whitespace?													*/
/*																			*/
int				is_wspc(char c);

/*																			*/
/*	Ensures that the write operation completed in full, if so, returns 1.	*/
/*																			*/
int				write_exact(int fd, void const *p, size_t size);

/*																			*/
/*	Removes quotes from a string without reallocation.						*/
/*																			*/
void			unquote_inplace(char *s);

#endif
