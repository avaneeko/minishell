#include "minishell.h"



static int is_alphanum(char c)
{
	return (c >= 'a' && c <= 'z')
		|| (c >= 'A' && c <= 'Z')
		|| (c >= '0' && c <= '9');
}

// q - current quote.
/*
TODO:
Properly Free resources on failure.
Handle $?, reject $1 etc.
*/
int	do_str_expansion(t_token **t, t_env const *env)
{
	char q;
	unsigned int i;
	t_astr a;

	i = 0;
	q = 0;
	if (!astr_create(&a))
		return (0);
	while ((*t)->token[i])
	{
		if ((*t)->token[i] == '$' && q != '\'')
		{
			// Variable expansion.
			unsigned int var_start = ++i;
			while (is_alphanum((*t)->token[i]) || (*t)->token[i] == '_')
				++i;
			if (var_start == i)
			{
				if (!astr_append2(&a, "$", 1))
				{
					astr_destroy(&a);
					return (0);
				}
				continue ;
			}
			unsigned int var_len = i - var_start;
			char *var_name = malloc(var_len + 1);
			if (!var_name)
			{
				astr_destroy(&a);
				return (0);
			}
			mcpy(var_name, (*t)->token + var_start, var_len);
			var_name[var_len] = '\0';
			t_epair pair;
			if (get_epair_by_key(env, var_name, &pair))
			{
				printf("Expanding variable: %s to %s\n", var_name, pair.value);
				if (!astr_append(&a, pair.value))
				{
					free(var_name);
					return (0);
				}
			}
			else
				printf("Expanding variable: %s to nothing.\n", var_name);
			free(var_name);
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
	
}
