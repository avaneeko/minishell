#ifndef ASTR_H
# define ASTR_H

/******************************************************************************/
/*	Appendable string.														  */
/******************************************************************************/

#ifndef ASTR_PREALLOC
# define ASTR_PREALLOC 512
#endif

typedef struct s_astr
{
	unsigned int	cap;	// Total allocated capacity in `s`.
	unsigned int	len;	// string `s` length, WITHOUT the \0 terminator.
	char			*s;		// The allocated string itself.
}	t_astr;

//
//	Creates an astr object.
//	Returns 1 on success, 0 otherwise.
//! `self` is not written in case of failure.
//
int		astr_create(t_astr *self);

//
//	Destroys astr object.
//!	It's forbidden to use the object after calling this function.
//
void	astr_destroy(t_astr const *self);

//
//	Appends `str` to the string.
//	Returns 1 on success, 0 otherwise.
//!	Destroys `self` on failure.
//
int		astr_append(t_astr *self, char const *str);

//
//	Same as astr_append(), but accepts a `buf` with `len` size instead of
//	null-terminated string as the append argument.
//!	Destroys `self` on failure.
//
int		astr_append2(t_astr *self, char const *buf, unsigned int len);

#endif
