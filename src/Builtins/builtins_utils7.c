/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_utils6.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 22:29:34 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/13 22:43:00 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"
#include <limits.h>		/* LLONG_MAX, LLONG_MIN */

/* For positive: lim = LLONG_MAX; for negative: lim = (unsigned)LLONG_MAX + 1 */
int	compute_limit_for_sign(int sign, unsigned long long *lim)
{
	if (lim == NULL)
		return (0);
	if (sign == 1)
		*lim = (unsigned long long)LLONG_MAX;
	else
		*lim = (unsigned long long)LLONG_MAX + 1ULL;
	return (1);
}

/* Accumulates one decimal digit with overflow guard under bound 'lim'. */
int	add_digit_check(unsigned long long *acc, unsigned long long lim, int d)
{
	unsigned long long	u;

	if (acc == NULL)
		return (0);
	if (d < 0 || d > 9)
		return (0);
	u = (unsigned long long)d;
	if (*acc > (lim - u) / 10ULL)
		return (0);
	*acc = (*acc * 10ULL) + u;
	return (1);
}

/* Converts accumulated magnitude to signed result, with LLONG_MIN case. */
void	assign_signed_result(int sign, unsigned long long acc,
		unsigned long long lim, long long *out)
{
	if (out == NULL)
		return ;
	if (sign == 1)
		*out = (long long)acc;
	else if (acc == lim)
		*out = LLONG_MIN;
	else
		*out = -(long long)acc;
}

/* Core digit parser: loop over digits, check overflow per step, then sign. */
int	parse_digits_core(const char *s, int start, int sign, long long *out)
{
	unsigned long long	acc;
	unsigned long long	lim;
	int					i;
	int					d;

	acc = 0;
	if (!compute_limit_for_sign(sign, &lim))
		return (0);
	i = start;
	while (s[i] != '\0')
	{
		d = s[i] - '0';
		if (!add_digit_check(&acc, lim, d))
			return (0);
		i += 1;
	}
	assign_signed_result(sign, acc, lim, out);
	return (1);
}

/* ------------------------ top-level integer parsing ---------------------- */

/* Public helper: parse optional sign, accumulate digits with overflow. */
int	parse_ll(const char *s, long long *out)
{
	int	sign;
	int	idx;

	if (!parse_sign(s, &idx, &sign))
		return (0);
	return (parse_digits_core(s, idx, sign, out));
}
