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
