/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtins_utils7.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/13 22:29:34 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 00:30:07 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"
#include <limits.h>		/* LLONG_MAX, LLONG_MIN */

/**
 * @brief Compute the overflow boundary for the given sign for parsing
 * 		to long long.
 * @param sign +1 for positive, -1 for negative.
 * @param lim Output: LLONG_MAX for positive, (unsigned)LLONG_MAX+1 for
 * 		negative.
 * @return 1 on success, 0 if lim is NULL.
 */
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

/**
 * @brief Add one digit d (0..9) into accumulator with overflow check
 * 		against lim.
 * @param acc In/out: accumulated magnitude.
 * @param lim Maximum allowed magnitude before overflow occurs.
 * @param d Decimal digit to add.
 * @return 1 if successful, 0 if overflow or invalid digit.
 */
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

/**
 * @brief Convert accumulated magnitude and sign to a long long, handling the
 * 		LLONG_MIN edge case.
 * @param sign +1 or -1 sign.
 * @param acc Accumulated magnitude.
 * @param lim Limit used to detect LLONG_MIN case.
 * @param out Output: resulting signed long long.
 */
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

/**
 * @brief Parse digits from s starting at index start with overflow checks and
 * 		produce a signed result.
 * @param s Input numeric string tail.
 * @param start Index of first digit.
 * @param sign +1 or -1 sign determined earlier.
 * @param out Output: parsed long long value.
 * @return 1 on success, 0 on overflow.
 */
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

/**
 * @brief Top-level parse for optional sign then digits into long long with
 * 		overflow checking.
 * @param s Input numeric string with optional leading sign.
 * @param out Output: parsed long long value.
 * @return 1 on success, 0 on invalid format or overflow.
 */
int	parse_ll(const char *s, long long *out)
{
	int	sign;
	int	idx;

	if (!parse_sign(s, &idx, &sign))
		return (0);
	return (parse_digits_core(s, idx, sign, out));
}
