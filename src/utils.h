/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 18:43:31 by losypenk          #+#    #+#             */
/*   Updated: 2025/07/30 13:18:47 by losypenk         ###   ########.fr       */
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
void	mcpy(void *dst, void const *src, size_t size);

/*																			*/
/*	Memory move.															*/
/*																			*/
void	mmov(void *src, void *dst, size_t size);

/*																			*/
/*	Memory compare.															*/
/*																			*/
int		mcmp(void const *s1, void const *s2, size_t size);

/*																			*/
/*	String length.															*/
/*																			*/
size_t	slen(char const *s);

/*																			*/
/*	Memory clone															*/
/*																			*/
void	*mclone(void const *p, size_t size);

/*																			*/
/*	String equal.															*/
/*	Returns 1 if the strings are equal, 0 otherwise.						*/
/*																			*/
int		streq(char const *a, char const *b);

/*																			*/
/*	String copy.															*/
/*																			*/
void	scpy(char *dst, char const *src);

#endif
