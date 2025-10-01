#include "minishell.h"

// From expand_last_err.c
int	expand_last_err(t_app *app);

//? Does this belong here? (x2)
static int is_alphanum(char c)
{
	return ((c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z')
		|| (c >= '0' && c <= '9'));
}

int	is_valid_var_name(char const *s)
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

int get_val_len(char const *s, unsigned int *out)
{
	unsigned int len;	// Length of the variable

	if (*s == '$' && is_valid_var_name(s) == 2)
	{
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
char const *get_expansion_contents(t_app *app, char const *s,
		unsigned int len)
{
	t_epair epair;

	if (len == 0)
		return ("$"); // Just a single $, no variable name.
	else if (len == 1 && s[0] == '?' && expand_last_err(app))
		return (app->last_exit_code_str); // WERONGGG!!!
	else if (get_epair_by_key2(&app->env, s, len, &epair))
		return (epair.value);
	else
		return (0);
}
