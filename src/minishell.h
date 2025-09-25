#ifndef MINISHELL_H
# define MINISHELL_H

# include <stdio.h> //! TODO: REMOVE THIS IN SHIPPING.

# include <unistd.h>
# include <sys/wait.h>
# include <sys/types.h>
# include <sys/stat.h>
# include <fcntl.h>
# include <signal.h>
# include <stdlib.h>
# include <readline/readline.h>
# include <readline/history.h>

# include "utils.h"
# include "astr.h"

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

// Max characters allowed in the prompt at a time.
#define PROMPT_CHAR_LIMIT 1024

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
	t_epair			*pairs;
}	t_env;

//
//	Appends the epair entry into the env.
//! Will free `env->pairs` and `pair` on failure.
//
int		try_append_epair(t_env *env, t_epair const *pair);

//
//	Removes the pair with matching `key`.
//	Returns 1 if epair with `key` was found and removed, 0 otherwise.
//
int		remove_epair_by_key(t_env *env, char const* key);

//
//	Lookup an epair with `key`.
//	Returns 1 if epair with `key` was found and written to `out`, 0 otherwise.
//! Do not destroy or free() the `out` pair.
//
int		get_epair_by_key(t_env const *env, char const* key, t_epair *out);

//
//	Just like get_epair_by_key() but with `key` and `key_len` instead
//	null terminated `key` argument.
//! Do not destroy or free() the `out` pair.
//
int		get_epair_by_key2(t_env const *env, char const* key,
		unsigned int key_len, t_epair *out);

//
//	Removes the pair at `idx` from the env list via remove-swap.
//
void	remove_epair_at_idx(t_env *env, unsigned int idx);

//
//	Destroys env, freeing every epair held by env and the env itself.
//
void	destroy_env(t_env *env);

//
//	Destroys all resources held by epair.
//	Assumes epair ptr is not malloc()'ed. (Current design)
//
void	destroy_epair(t_epair const *pair);

//
//	TODO: Document.
//
int	create_env_from_envp(char const **envp, t_env *out_env);

//
// Parses all of envp into `t_env`
//
int		parse_envp(t_env *env, char const **envp);

// Initial amount of entries reserved by `t_env`, in entries.
# ifndef ENV_MEM_RESERVE
#  define ENV_MEM_RESERVE 1024u
# endif

// Amount of entries the env list is grown if it ever becomes full.
# ifndef ENV_MEM_GROW_SIZE
#  define ENV_MEM_GROW_SIZE 512u
# endif

#define ORIGIN_EXPORT 42
#define ORIGIN_ENV -42

//
//	Expansion.
//

// Holds all expansion state/information.
typedef struct s_exp
{
	t_astr	a;				  // Appendable string for currently expanded token.
	t_token_list **list;	  // The token list.
	t_env const *env;		  // Environment variables.
}	t_exp;

//
//	Checks whether the string `s` contains a valid start sequence of a variable.
//	Returns:
//	0 - invalid variable name
//	1 - valid standard variable name
//	2 - special variable (currently only $?)
//
int	is_valid_var_name(char const *s);

// Get length of the variable.
// Examples (return meaning `*out =`):
// Given s -> "$VAR something" it should set `out` to 4. Ret = 1
// Given s -> "$" it should set `out` to 1. (single $ becomes a $) Ret = 1
// Given s -> "something" it should return 0. (no $ found, not a variable)
//! This does not handle $1 to $9 - positional arguments, out of scope.
// TODO: Handle $? - last exit code.
int get_val_len(char const *s, unsigned int *out);

//
//	Gets the contents based on the environment key `s`
//*	`s` cannot be null.
//
char const *get_expansion_contents(t_env const* env, char const *s,
		unsigned int len);

// struct s_app;
typedef struct s_app t_app;

//
//	Tokenizer.
//
int	tokenize(char *str, t_token_list *list);

//
//	Token expansion.
//
int	expand(t_token_list **list, t_env const *env);

//
//	Post-expansion token splitting.
//
int token_resplit(t_app *app);

//
//	All things application.
//

//
//	Command array
//

// Forward declare.
typedef struct s_command t_command;

//$ Command array is the execution chain from the tokenizers output.
//$ This executiuon chain is later executed by the execution unit itself.
typedef struct s_cmdarr
{
	t_command *cmds;
	int unsigned cap;
	int unsigned len;
} t_cmdarr;

//
//	Creates a new command array. Initial capacity is set to CMDARR_INIT_CAP.
//	Returns 1 on success, 0 otherwise.
//
int cmdarr_create(t_cmdarr *self);

//
//	Destroys the command array, freeing all allocated memory.
//
void cmdarr_destroy(t_cmdarr *self);

//
//	Appends a command to the command array.
//	Will resize the command array if needed.
//	If destroy_on_fail is set to 1, the command will be freed using
//	destroy_command() and self with destroy_command() on failure.
//	cmd is copied, but its contents are not deep-copied.
//	Returns 1 on success, 0 otherwise.
//
int cmdarr_append(t_cmdarr *self, t_command *cmd, int const destroy_on_fail);

//
//	Application state.
//
typedef struct s_app
{
	t_env			env;
	t_token_list	*token_list;
	t_cmdarr		exec;
	int				heredocs[16];
	int unsigned	cur_hd;
	char			*cur_hd_name;
}	t_app;

int		app_create(int argc, char const **argv, char const **envp,
	t_app *out);

void	app_destroy(t_app *app);

// Closes all open fd's of app->heredoc.
void app_reset_heredocs(t_app *app);

//
//	Heredocument.
//

//
//	Prompt the user for all the here documents inside the tokens.
//
int	prompt_heredoc(t_app *app);

//
//	Execution.
//

// Initial amount of entries reserved by `t_cmdarr`, in entries.
# ifndef CMDARR_MEM_RESERVE
#  define CMDARR_MEM_RESERVE 1024u
# endif

int build_exec(t_app *app);
typedef struct s_redir
{
	int				type;     // e.g., TOKEN_REDIRECT_INPUT, TOKEN_REDIRECT_OUTPUT, etc.
	char			*target;  // filename or heredoc delimiter
	struct s_redir	*next;
}   t_redir;

typedef struct s_command
{
	char				**argv;
	t_redir				**redirs;     // Linked list of redirections
	int					infile;      // File descriptor for redirected input or -1
    int					outfile;     // File descriptor for redirected output or -1
	int					is_builtin;  // 1 if is builtin, 0 if not
	struct s_command	*next;      // Next command in pipeline
}   t_command;

#endif
