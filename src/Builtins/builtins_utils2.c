/*
** ms_parse_ll: Parse a signed long long like bash does for `exit`.
** - Accepts leading/trailing spaces.
** - Accepts optional '+' or '-' sign.
** - Requires all remaining chars to be digits (or spaces at end).
** - Detects overflow and reports failure.
** - No ternary operators, no for loops, helpers kept short per NormV3.
** Why this matters: 42 Norm forbids ternary and enforces line limits, so we
** structure parsing into compact helpers to stay compliant [NormV3.pdf] [attached_file:2].
*/

#include <limits.h> /* LLONG_MIN, LLONG_MAX */
#include <stddef.h> /* size_t */

/* Return 1 if c is ASCII space or control whitespace, else 0. */
static int	ms_is_space(char c)
{
	if (c == ' ')
		return (1);
	if (c >= 9 && c <= 13)
		return (1);
	return (0);
}

/* Skip leading spaces starting at index i; return next index. */
static int	ms_skip_spaces(char const *s, int i)
{
	while (s[i] && ms_is_space(s[i]))
		i++;
	return (i);
}

/* Read optional sign; sets *neg = 1 for '-', 0 otherwise; returns next index. */
static int	ms_parse_sign(char const *s, int i, int *neg)
{
	*neg = 0;
	if (s[i] == '+' || s[i] == '-')
	{
		if (s[i] == '-')
			*neg = 1;
		i++;
	}
	return (i);
}

/*
** Accumulate digits into *acc with overflow checks.
** - Requires at least one digit at entry index.
** - Uses unsigned math and checks against LLONG_MAX (and +1 for negative).
** Returns next index on success, or -1 on error.
*/
static int	ms_accumulate_digits(char const *s, int i, int neg,
	unsigned long long *acc)
{
	*acc = 0;
	if (s[i] < '0' || s[i] > '9')
		return (-1);
	while (s[i] >= '0' && s[i] <= '9')
	{
		if (*acc > (unsigned long long)LLONG_MAX / 10ULL)
			return (-1);
		*acc = (*acc * 10ULL) + (unsigned long long)(s[i] - '0');
		if (!neg && *acc > (unsigned long long)LLONG_MAX)
			return (-1);
		if (neg && *acc > (unsigned long long)LLONG_MAX + 1ULL)
			return (-1);
		i++;
	}
	return (i);
}

/*
** Public parser: returns 1 on success and writes the result in *out, else 0.
** - Accepts trailing spaces but nothing else after digits.
** - Maps unsigned accumulator back to signed long long, including LLONG_MIN.
*/
int	ms_parse_ll(char const *s, long long *out)
{
	unsigned long long	acc;
	int					neg;
	int					i;

	if (!s)
		return (0);
	i = 0;
	i = ms_skip_spaces(s, i);
	i = ms_parse_sign(s, i, &neg);
	i = ms_accumulate_digits(s, i, neg, &acc);
	if (i < 0)
		return (0);
	i = ms_skip_spaces(s, i);
	if (s[i] != '\0')
		return (0);
	if (neg)
	{
		if (acc == (unsigned long long)LLONG_MAX + 1ULL)
			*out = LLONG_MIN;
		else
			*out = -(long long)acc;
	}
	else
		*out = (long long)acc;
	return (1);
}

/*
Explanation notes:
- No ternary operator is used anywhere to comply with NormV3 forbidden items [attached_file:2].
- No for loops are used; only while loops, also satisfying NormV3 constraints [attached_file:2].
- The main function body is kept compact; helpers keep per-function line counts within limits [attached_file:2].
*/
