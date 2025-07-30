/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 18:41:55 by losypenk          #+#    #+#             */
/*   Updated: 2025/07/30 13:43:29 by losypenk         ###   ########.fr       */
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
	unsigned int	cap;	// Capacity of .tok
	unsigned int	len;	// Count of valid tokens in .tok
	t_token			*tok[];	// Token ptr array.
}	t_token_list;

//
//	Instantiate a new token list, with set capacity.
//	Returns 1 on success, 0 otherwise.
//
int		new_token_list(unsigned int capacity, t_token_list **out);

//
//	Destroys the token list.
//	Releases all resources held by t_token_list.
//	This does not release the tokens that list refernces, only the list itself.
//*	list is unusable after this function.
//
void	destroy_token_list(t_token_list const *list);

//
//	Copies the token list. dst must have enough space for the copy.
//
void	copy_token_list(t_token_list const *src, t_token_list *dst);

//
//	Clones the token list, allocating resources for the newly created clone.
//*	This does not clone the tokens, only their references inside the list.
//	Returns 1 on success, 0 otherwise.
//
int		clone_token_list(t_token_list const *list, t_token_list **out);

//
//	Resizes the token list.
//	Always free()'s the old *list, even on failure.
//	Returns 1 on success, 0 otherwise.
//
int		resize_token_list(t_token_list **list, unsigned int new_capacity);

//
//	Creates an allocated token with type `type` and `tok` contents.
//*	`tok` pointer is optional.
//	If `tok` is null, the token will only have the type and no contents.
//	Writes the new token to *out, only on success.
//	Returns 1 on success, 0 otherwise.
//
int		create_token(enum e_token_type type, char const *tok, t_token **out);

//
//	Destroy token. Releases all resources held by token.
//	Token is rendered unsable after this function.
//
void	destroy_token(t_token const *token);

#endif
