
#ifndef BUILTINS_H
# define BUILTINS_H

# include "minishell.h"
# include <string.h>
# include <linux/limits.h>
# include <stdio.h>
# include <unistd.h>
# include <stdlib.h>
# include "../Execution/execution_utils.h"


// Change the current working directory to argv[1] (or HOME if none)
int	builtin_cd(char **argv, t_env *env);

// Print its arguments to stdout, separated by spaces, ending with newline
int	builtin_echo(char **argv);

// Print the current working directory to stdout
int	builtin_pwd(void);

// Print the environment to stdout (one KEY=VALUE pair per line)
int	builtin_env(t_env *env);

// Add or replace exported environment variables
int	builtin_export(char **argv, t_env *env);

// Unset (remove) environment variables by name
int	builtin_unset(char **argv, t_env *env);

// Exit the shell. Accepts optional numeric status argument.
// int	builtin_exit(char **argv);
int	builtin_exit(t_app *app, char **argv);

// env helpers
int env_find_key(t_env *env, char *key);
int env_set(t_env *env, char *key, char *value, int origin);
int env_unset(t_env *env, char *key);

// lifbt function replica
char	*ft_strdup(const char *s1);

int	is_builtin(const char *cmd);
// int	exec_builtin(char **argv, t_env *env);

int	exec_builtin(t_app *app, char **argv, t_env *env);

/* helper function for builtin exit */
void	app_destroy_safe(t_app *app);

#endif
