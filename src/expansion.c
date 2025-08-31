#include "minishell.h"

/*
TODO:
Free resources on failure.
*/

// q - current quote.
int	do_str_expansion(t_token **t, t_env const *env)
{
	char q;
	unsigned int i;
	t_astr a;
}

int	expand(t_token_list **list, t_token_list **new_list, t_env const *env)
{
	int				ok;
	unsigned int	i;

	ok = 1;
	i = 0;
	while (ok && i < (*list)->len)
	{
		if ((*list)->tok[i]->type == TOKEN_WORD)
			ok &= do_str_expansion((*list)->tok[i], new_list, env);
	}
	
}
