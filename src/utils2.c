/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils2.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 19:06:24 by losypenk          #+#    #+#             */
/*   Updated: 2025/07/25 19:36:10 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.h"

/*																			*/
/*	String equal.															*/
/*	Returns 1 if the strings are equal, 0 otherwise.						*/
/*																			*/
int		streq(char const *a, char const *b)
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
