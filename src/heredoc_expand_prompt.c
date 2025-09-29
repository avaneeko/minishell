#include "minishell.h"

// This file declares:
static int	expand_variable(char *str, t_app *app, t_astr *a,
							unsigned int **arg);
static int	err_expand_prompt_astr_failure(void *input_ptr);
static int	handle_char(char const *str, t_astr *a, int unsigned *i);
int	expand_prompt(t_app *app, char **input);

// Function that expands a heredoc prompt.
// Will free `input` on failure.
// Returns 1 on success, 0 otherwise.
int	expand_prompt(t_app *app, char **input)
{
	unsigned int	var_len;
	int unsigned	i;
	t_astr			a;

	if (!astr_create(&a))
		return (err_expand_prompt_astr_failure(*input));
	i = 0;
	while ((*input)[i])
	{
		if (get_val_len(*input + i, &var_len))
		{
			if (!expand_variable(*input, app, &a,
					(unsigned int *[]){&i, &var_len}))
				return (0);
		}
		else if (!handle_char(*input, &a, &i))
			return (err_expand_prompt_astr_failure(NULL));
	}
	free(*input);
	*input = a.s;
	return (1);
}

// arg[0] is *i
// arg[1] is *var_len
static int	expand_variable(char *str, t_app *app, t_astr *a,
							unsigned int **arg)
{
	char *const	var_name = malloc(arg[1][0] + 1);

	if (!var_name)
	{
		astr_destroy(a);
		return (0);
	}
	mcpy(var_name, str + arg[0][0], arg[1][0]);
	var_name[arg[1][0]] = '\0';
	char const *val = get_expansion_contents(app, str + arg[0][0] + 1,
			arg[1][0] - 1);
	if (val && !astr_append2(a, val, slen(val)))
	{
		free(var_name);
		astr_destroy(a);
		return (0);
	}
	free(var_name);
	arg[0][0] += arg[1][0];
	return (1);
}

static int	err_expand_prompt_astr_failure(void *input_ptr)
{
	free(input_ptr);
	write(2, "minishell: AppendableString failure: Out of memory"
		". Aborting prompt.\n", 69);
	return (0);
}

// Handles characters that are not part of the variable expansion.
// Handles quotes as well.
//! Will free `str` on failure.
// Returns 1 on success, 0 otherwise.
// haha we actually do want to append quotes, but not handle them anyhow.
static int	handle_char(char const *str, t_astr *a, int unsigned *i)
{
	if (!astr_append2(a, str + *i, 1))
	{
		free((void *)str);
		return (0);
	}
	*i += 1;
	return (1);
}
