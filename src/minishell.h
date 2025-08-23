/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   minishell.h                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/25 18:41:55 by losypenk          #+#    #+#             */
/*   Updated: 2025/08/23 18:05:59 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h> //! TODO: REMOVE THIS IN SHIPPING.

# include <unistd.h>
# include <sys/wait.h>
# include <signal.h>
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
//!	.token might be missing from certain .type tokens. That's intentional.
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
//	This does not release the tokens that list references, only the list itself.
//*	list is unusable after this function.
//
void	destroy_token_list(t_token_list const *list);

//
//	Destroys the token list and all of it's tokens.
//*	list and its tokens are rendered unusable after this function.
//
void	destroy_token_list_deep(t_token_list const *list);

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
//	Empties the entire token list, freeing all it's tokens and their references.
//
void	clear_token_list(t_token_list *list);

//
//	Resizes the token list.
//	Always free()'s the old *list, even on failure.
//	Returns 1 on success, 0 otherwise.
//
int		resize_token_list(t_token_list **list, unsigned int new_capacity);

//
//	Appends the new token pointer to the list, might grow the `*list` if it's
//	capacity is too small for appending.
//!	Will free the `*list` and `token` on failure.
//	Returns 1 on success, 0 on failure.
//
int		append_token_list(t_token_list **list, t_token *token);

//
//	Creates an allocated token with type `type` and `tok` contents.
//*	`tok` pointer is optional.
//	If `tok` is null, the token will only have the type and no contents.
//	Writes the new token to *out, only on success.
//	Returns 1 on success, 0 otherwise.
//
int		create_token(enum e_token_type type, char const *tok, t_token **out);

//
//	Same as create_token, but accepts a pointer with a size to allow for
//	strings with no null termination as the token contents.
//	Writes the new token to *out, only on success.
//	Returns 1 on success, 0 otherwise.
//
int		create_token2(enum e_token_type type, char const *tok, size_t tok_len,
		t_token **out);

//
//	Destroy token. Releases all resources held by token.
//	Token is rendered unusable after this function.
//
void	destroy_token(t_token const *token);

//
//	Modifies the token's contents by recreating the token with appropriate size.
//! It's forbidden to call this function on tokens that do not have any token
//!	contents, such as e_token_type::TOKEN_PIPE.
//!	`new_contents` cannot be null.
//*	Always free()'s the `*token`, even on failure, unless no reallocation was
//*	required.
//* `new_contents` are copied into the token, not referenced.
//	Writes the output to `*token` or nulls it on failure.
//	Returns 1 on success, 0 otherwise.
//
int		modify_token(t_token **token, char const *new_contents);

//
//	Environment.
//

// Initial amount of entries reserved by `t_env`, in entries.
# ifndef ENV_MEM_RESERVE
#  define ENV_MEM_RESERVE 1024
# endif

//
//	Environmental pair of key and value.
//
typedef struct s_epair
{
	char	*key;
	char	*value;
	int		origin;	// Specifies whether it came from env or export.
}	t_epair;

typedef struct s_env
{
	unsigned int	len;
	unsigned int	cap;
	t_epair			pairs[];
}	t_env;

//
//	Destroys env, freeing every epair held by env and the env itself.
//
void	destroy_env(t_env const *env);

//
//	Destroys all resources held by epair.
//	Assumes epair ptr is not malloc()'ed. (Current design)
//
void	destroy_epair(t_epair const *pair);

//
// Parses all of envp into `t_env`
//
int		parse_envp(t_env *env, char const **envp);

//
//	All things application.
//

//
//	Application state.
//
typedef struct s_app
{
	t_env	*env;
}	t_app;

int		app_create(int argc, char const **argv, char const **envp);

#endif
