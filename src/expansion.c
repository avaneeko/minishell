#include "minishell.h"

//? Does this belong here?
static int is_alphanum(char c)
{
	return ((c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z')
		|| (c >= '0' && c <= '9'));
}

// Validates if the variable name is valid according to shell rules
// Returns:
// 0 - invalid variable name
// 1 - valid standard variable name
// 2 - special variable (currently only $?)
static int is_valid_var_name(char const *s)
{
	// Check for $? (exit status)
	if (s[0] == '$' && s[1] == '?')
		return 2;

	// Check if there's a $ and at least one character after it
	if (s[0] != '$' || s[1] == '\0')
		return 0;

	// Variable name can't start with a number
	if (s[1] >= '0' && s[1] <= '9')
		return 0;

	// First character must be alpha or underscore
	if (!((s[1] >= 'a' && s[1] <= 'z') || 
			(s[1] >= 'A' && s[1] <= 'Z') || 
			s[1] == '_'))
		return 0;

	return 1;
}

// Get length of the variable.
// Examples (return meaning `*out =`):
// Given s -> "$VAR something" it should set `out` to 4. Ret = 1
// Given s -> "$" it should set `out` to 1. (single $ becomes a $) Ret = 1
// Given s -> "something" it should return 0. (no $ found, not a variable)
//! This does not handle $1 to $9 - positional arguments, out of scope.
// TODO: Handle $? - last exit code.
int get_val_len(char const *s, unsigned int *out)
{
	unsigned int len;	// Length of the variable

	if (*s == '$' && is_valid_var_name(s) == 2)
	{
		__builtin_debugtrap(/* UNIMPLEMENTED */);
		*out = 2; // $?
		return (1);
	}
	else if (*s == '$' && is_valid_var_name(s) == 1)
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

// arg[0] is *i
// arg[1] is *var_len
static int	expand_variable(t_token *token, t_env const *env, t_astr *a, 
							unsigned int **arg)
{
	char *const	var_name = malloc(arg[1][0] + 1);

	if (!var_name)
	{
		astr_destroy(a);
		return (0);
	}
	mcpy(var_name, token->token + arg[0][0], arg[1][0]);
	var_name[arg[1][0]] = '\0';
	char const *val = get_expansion_contents(env, token->token + arg[0][0] + 1,
			arg[1][0] - 1);
	if (val)
	{
		if (!astr_append2(a, val, slen(val)))
		{
			free(var_name);
			astr_destroy(a);
			return (0);
		}
	}
	free(var_name);
	arg[0][0] += arg[1][0];
	return (1);
}

static int	handle_quoted_char(t_token *token, t_astr *a, char *q, unsigned int *i)
{
	if (token->token[*i] == '\'' || token->token[*i] == '"')
	{
		if (*q == 0)
			*q = token->token[*i];
		else if (*q == token->token[*i])
			*q = 0;
	}
	if (!astr_append2(a, token->token + *i, 1))
		return (0);
	*i += 1;
	return (1);
}

// In do_str_expansion:



// q - current quote.
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
			if (!expand_variable(*t, env, &a, (unsigned int *[]){&i, &var_len}))
				return (0);
		}
		else
		{
			if (!handle_quoted_char(*t, &a, &q, &i))
				return (0);
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
