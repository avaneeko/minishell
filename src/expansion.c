#include "minishell.h"

static int is_alphanum(char c)
{
	return (c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z')
		|| (c >= '0' && c <= '9');
}

// Get length of the variable.
// Examples (return meaning `*out =`):
// Given s -> "$VAR something" it should set `out` to 4. Ret = 1
// Given s -> "$" it should set `out` to 1. (single $ becomes a $) Ret = 1
// Given s -> "something" it should return 0. (no $ found, not a variable)
//! This does not handle $1 to $9 - positional arguments, out of scope.
// TODO: Reject s -> $9startingwithnumber
// TODO: Handle $? - last exit code.
int get_val_len(char const *s, unsigned int *out)
{
	unsigned int len;	// Length of the variable

	if (*s == '$')
	{
		len = 1;
		s++;
		while (is_alphanum(*s) || *s == '_')
		{
			len++;
			s++;
		}
		*out = len;
		return (1);
	}
	return (0);
}


// s cannot be null.
char const *get_expansion_contents(t_env const* env, char const *s,
		unsigned int len)
{
	t_epair epair;

	if (len == 0)
		return ("$"); // Just a single $, no variable name.
	else if (get_epair_by_key2(env, s, len, &epair))
		return (epair.value);
	else
		return (0);
}

// q - current quote.
/*
TODO:
Properly Free resources on failure.
Handle $?, reject $1 etc.
*/
int	do_str_expansion(t_token **t, t_env const *env)
{
	char			q;
	unsigned int	i;
	unsigned int	var_len;
	t_astr			a;

	i = 0;
	q = 0;
	if (!astr_create(&a))
		return (0);
	while ((*t)->token[i])
	{
		if (q != '\'' && get_val_len((*t)->token + i, &var_len))
		{
			// Variable expansion.
			char *var_name = malloc(var_len + 1);
			if (!var_name)
			{
				astr_destroy(&a);
				return (0);
			}
			mcpy(var_name, (*t)->token + i, var_len);
			var_name[var_len] = '\0';
			char const *val = get_expansion_contents(env, (*t)->token + i + 1, var_len - 1);
			if (val)
			{
				if (!astr_append2(&a, val, slen(val)))
				{
					free(var_name);
					astr_destroy(&a);
					return (0);
				}
			}
			free(var_name);
			i += var_len;
		}
		else
		{
			if ((*t)->token[i] == '\'' || (*t)->token[i] == '"')
			{
				if (q == 0)
					q = (*t)->token[i];
				else if (q == (*t)->token[i])
					q = 0;
			}
			if (!astr_append2(&a, (*t)->token + i, 1))
				return (0);
			i++;
		}
	}
	return (modify_token(t, a.s));
}

int	expand(t_token_list **list, t_env const *env)
{
	int				ok;
	unsigned int	i;

	ok = 1;
	i = 0;
	while (ok && i < (*list)->len)
	{
		if ((*list)->tok[i]->type == TOKEN_WORD)
			ok &= do_str_expansion((*list)->tok + i, env);
		i++;
	}
	return (ok);
}
