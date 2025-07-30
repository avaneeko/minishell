/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   token.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/30 13:13:16 by losypenk          #+#    #+#             */
/*   Updated: 2025/07/30 13:34:09 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int create_token(enum e_token_type type, char const *tok, t_token **out)
{
	t_token	*mem;

	if (tok)
		mem = malloc(sizeof(t_token) + (slen(tok) + 1));
	else
		mem = malloc(sizeof(t_token));
	if (!mem)
		return (0);
	mem->type = type;
	if (tok)
		scpy(mem->token, tok);
	*out = mem;
	return (1);
}
