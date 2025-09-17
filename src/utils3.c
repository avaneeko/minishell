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
