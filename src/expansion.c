#include "minishell.h"

// arg[0] is *i
// arg[1] is *var_len
// HACK: Using fake app to call get_expansion_contents, as in this case we don't
// have access to the real app struct, but we do have access to env, which is
// the only thing we need.
static int	expand_variable(t_token *token, t_env const *env, t_astr *a,
							unsigned int **arg)
{
	char *const	var_name = malloc(arg[1][0] + 1);
	t_app const fake_app = (t_app){ .env = *env };

	if (!var_name)
	{
		astr_destroy(a);
		return (0);
	}
	mcpy(var_name, token->token + arg[0][0], arg[1][0]);
	var_name[arg[1][0]] = '\0';
	char const *val = get_expansion_contents((void *)&fake_app, token->token + arg[0][0] + 1,
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

static int	handle_quoted_char(t_token *token, t_astr *a, char *q,
		unsigned int *i)
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

int	expand(t_app *app, t_token_list **list, t_env const *env)
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
