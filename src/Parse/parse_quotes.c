#include "minishell.h"
#include <stdlib.h>

static int	handle_quote(char c, char *q)
{
	if ((c == '\'' || c == '"') && (!*q || *q == c))
	{
		if (*q == 0)
			*q = c;
		else
			*q = 0;
		return (1); // quote char handled (should be skipped)
	}
	return (0); // not a quote char or no action
}

static char	*remove_quotes_str(const char *src)
{
	size_t	i;
	size_t	j;
	char	q;
	char	*dst;

	dst = malloc(slen(src) + 1);
	if (!dst)
		return (NULL);
	i = 0;
	j = 0;
	q = 0;
	while (src[i])
	{
		if (handle_quote(src[i], &q))
		{
			i++;
			continue ;
		}
		dst[j++] = src[i++];
	}
	dst[j] = 0;
	return (dst);
}

int	remove_quotes_tokenlist(t_token_list *list)
{
	unsigned int	i;
	char			*unquoted;

	i = 0;
	while (i < list->len)
	{
		if (list->tok[i]->type == TOKEN_WORD)
		{
			unquoted = remove_quotes_str(list->tok[i]->token);
			if (!unquoted)
				return (0);
			modify_token(&(list->tok[i]), unquoted);
			free(unquoted);
		}
		i++;
	}
	return (1);
}
