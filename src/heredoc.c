#include "minishell.h"

int	is_bad_heredoc(t_app *app, unsigned int i)
{
	if (app->token_list->tok[i]->type == TOKEN_HEREDOC
		&& i + 1 < app->token_list->len
		&& app->token_list->tok[i + 1]->type != TOKEN_WORD)
	{
		// Heredoc followed by non-word.
		// TODO: non-fatal.
		write(2, "FATAL: Heredoc followed by non-word.\r\n", sizeof "FATAL: Heredoc followed by non-word.\r\n");
		_exit(191);
		return (1);
	}
	else if (app->token_list->tok[i]->type == TOKEN_HEREDOC
		&& i + 1 >= app->token_list->len)
	{
		// Heredoc hanging at the end of the list.
		// TODO: non-fatal.
		write(2, "FATAL: Heredoc hanging at the end of the list.\r\n", sizeof "FATAL: Heredoc hanging at the end of the list.\r\n" - 1);
		_exit(191);
		return (1);
	}
	return (0);
}

int	is_good_heredoc(t_app *app, unsigned int i)
{
	return (app->token_list->tok[i]->type == TOKEN_HEREDOC
			&& i + 1 < app->token_list->len
			&& app->token_list->tok[i + 1]->type == TOKEN_WORD);
}

// TODO: Make this respect TMPDIR and create heredocs there.
//! Designed for idx bound to 0..15
char const	*get_heredoc_filename(int unsigned idx)
{
	char const *const names[16] = {".hd0", ".hd1", ".hd2", ".hd3", ".hd4",
		".hd5", ".hd6", ".hd7", ".hd8", ".hd9", ".hd10", ".hd11", ".hd12",
		".hd13", ".hd14", ".hd15"};

	return (names[idx]);
}

// Try to open a new heredoc. Opens a file to be used for heredoc.
// idx - heredoc fd.
int	try_open_heredoc(t_app *app, int /*out*/ *idx)
{
	int unsigned	i;

	i = ~0;
	while (++i < 16)
	{
		if (app->heredocs[i] == -1)
			break;
	}
	if (i >= 16)
	{
		write(2, "minishell: maximum here-document count exceeded", 47);
		app_destroy(app);
		exit(1);
	}
	app->cur_hd = i;
	unlink(get_heredoc_filename(i));
	app->heredocs[i] = open(get_heredoc_filename(i), O_RDWR | O_CREAT | O_EXCL,
		0600);
	if (app->heredocs[i] == -1)
		return (0); // Convey error?
	*idx = app->heredocs[i];
	// can't unlink here cuz the file needs to be re-opened later.
	return (1);
}

// Util to write heredoc input and free it with error reporting.
// Always frees `input`
// Closes `*fd` in case of failure.
static int	write_heredoc(int *fd, char *input, int const nl)
{
	if (write_exact(*fd, input, slen(input)))
	{
		if (nl && write_exact(*fd, "\n", 1))
			;
		else
		{
			write(2, "minishell: Failure occurred when writing to a heredoc"
				" file descriptor. Aborting prompt.\n", 88);
			close(*fd);
			free(input);
			return ((*fd = -1) == 0);
		}
		free(input);
		return (1);
	}
	else
	{
		write(2, "minishell: Failure occurred when writing to a heredoc file"
			" descriptor. Aborting prompt.\n", 88);
		close(*fd);
		free(input);
		return ((*fd = -1) == 0);
	}
}

static int	err_expand_prompt_astr_failure()
{
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

// Inplace quote removal, helps to avoid the malloc.
static void	unquote_inplace(char *s)
{
	char	*w;

	w = s;
	while (*s)
	{
		if (*s != '\'' && *s != '"')
			*w++ = *s;
		s++;
	}
	*w = 0;
}

// arg[0] is *i
// arg[1] is *var_len
static int	expand_variable(char *str, t_env const *env, t_astr *a,
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
	char const *val = get_expansion_contents(env, str + arg[0][0] + 1,
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

// Function that expands a heredoc prompt.
// Will free `input` on failure.
// Returns 1 on success, 0 otherwise.
int	expand_prompt(t_env const *env, char **input)
{
	unsigned int	var_len;
	int unsigned	i;
	t_astr			a;

	if (!astr_create(&a))
		return (err_expand_prompt_astr_failure());
	i = 0;
	while (input[i])
	{
		if (get_val_len(*input + i, &var_len))
		{
			if (!expand_variable(*input, env, &a,
					(unsigned int *[]){&i, &var_len}))
				return (0);
		}
		else if (!handle_char(*input, &a, &i))
			return (err_expand_prompt_astr_failure());
	}
	free(*input);
	*input = a.s;
	return (1);
}

int	heredoc_input_tty(t_app *app, int *fd, char const *heredoc_end, int exp)
{
	char	*input;

	while (1)
	{
		// TODO [MIN-32]: Signals for readline here.
		input = readline("> ");
		if (!input)
		{
			// CTRL+D
			// Simply stop. Keep whatever was written to the file `*fd`
			// Discard current `input` prompt.
			close(app->heredocs[app->cur_hd]);
			app->heredocs[app->cur_hd] = open(get_heredoc_filename(app->cur_hd),
				O_RDONLY, 0600);
			unlink(get_heredoc_filename(app->cur_hd));
			free(input);
			return (1);
		}
		else if (streq(input, heredoc_end))
		{
			// Found end of heredoc.
			free(input);
			return (1);
		}
		else if (input && exp)
		{
			// Expand and write the input.
			expand_prompt(&app->env, &input) && write_heredoc(fd, input, 1);
		}
		else
		{
			// Simply write the input, no expansion.
			if (!write_heredoc(fd, input, 1))
				return (0);
		}
	}
	return (1);
}

// TODO [MIN-33]: Implement heredoc_input_fd()
int	heredoc_input_fd(t_app *app, int *fd, char const *heredoc_end, int exp)
{
	(void)app; (void)fd; (void)heredoc_end; (void)exp;
	__builtin_trap(/*UNIMPLEMENTED*/);
}

//* will close *fd on failure.
int	get_heredoc_input(t_app *app, int *fd, char *heredoc_end, int exp)
{
	unquote_inplace(heredoc_end);

	if (isatty(STDIN_FILENO) && heredoc_input_tty(app, fd, heredoc_end, exp))
		return (1);
	else if (!isatty(STDIN_FILENO)
		&& heredoc_input_fd(app, fd, heredoc_end, exp))
		return (1);
	return (0); // Failure!
}

/*
unused.
int	expand_heredoc(t_env const *env, int should_expand)
{
	t_astr	a;

	if (!should_expand)
		return (1);
	if (!astr_create(&a))
		return (0);

	return (1);
}
*/

// Used to figure out whether heredoc needs expansion.
static int	has_quotes(char const *s)
{
	while (*s)
	{
		if (*s=='\'' || *s=='"')
			return (1);
		s++;
	}
	return (0);
}

// heredoc_end - delimiter.
int do_prompt(t_app *app, t_token *hd, t_token *hd_end, char *heredoc_end)
{
	int	fd;
	int const exp = !has_quotes(heredoc_end); // is expansion required?

	if (try_open_heredoc(app, &fd) && get_heredoc_input(app, &fd, heredoc_end,
			exp))
	{
		hd->token[0] = fd; // Save fd inside the token.
		hd_end->type = TOKEN_UNDEFINED; // Mark token as spent.
		return (1); // Success!
	}
	else
		return (0); // Failure!
}

int	prompt_heredoc(t_app *app)
{
	unsigned int	i;

	i = ~0;
	while (++i < app->token_list->len) {
		if (is_good_heredoc(app, i))
		{
			// Prompt for heredoc with
			// app->token_list->tok[i] and app->token_list->tok[i + 1]
			do_prompt(app, app->token_list->tok[i], app->token_list->tok[i + 1],
				app->token_list->tok[i + 1]->token);
		}
		else if (is_bad_heredoc(app, i))
		{

		}
		// Otherwise it has nothing to do with heredoc, do nothing.
	}
	return (1);
}
