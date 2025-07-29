/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 18:41:55 by losypenk          #+#    #+#             */
/*   Updated: 2025/07/29 15:16:12 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h> //! TODO: REMOVE THIS IN SHIPPING.

# include <unistd.h>
# include <sys/wait.h>
# include <stdlib.h>
# include <readline/readline.h>
# include <readline/history.h>

# include "utils.h"

enum e_token_type
{
	TOKEN_UNDEFINED,
	TOKEN_WORD,
	TOKEN_PIPE,
	TOKEN_REDIRECT_INPUT,
	TOKEN_REDIRECT_OUTPUT,
	TOKEN_REDIRECT_OUTPUT_APPEND,
	TOKEN_HEREDOC,
	TOKEN_VARIABLE
};

//
//	Defines a token.
//!	.token might be missing from certain .type tokens.
//
typedef struct s_token
{
	unsigned int	type;		// Token type from e_token_type
	char			token[];	// Token's contents.
}	t_token;

typedef struct s_token_list
{
	t_token			*tok;	// Token ptr array.
	unsigned int	len;	// It's length.
}	t_token_list;

#endif
