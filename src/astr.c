#include "astr.h"
#include <stdlib.h>
#include "utils.h"

int		astr_create(t_astr *self)
{
	void *const	mem = malloc(ASTR_PREALLOC);

	if (!mem)
		return (0);
	self->cap = ASTR_PREALLOC;
	self->len = 0;
	self->s	= mem;
	self->s[0] = 0;
	return (1);
}

void	astr_destroy(t_astr const *self)
{
	free(self->s);
}

int		astr_grow(t_astr *self, unsigned int size)
{
	void *const	mem = mclone_grow(self->s, self->len + 1,
			self->cap += size);

	free(self->s);
	return ((self->s = mem) != 0);
}

int		astr_append(t_astr *self, char const *str)
{
	size_t	str_len = slen(str);

	if (self->len + str_len + 1 > self->cap || astr_grow(self,
			ASTR_PREALLOC + str_len))
	{
		// TODO: Concat.
	}
	return (0);
}
