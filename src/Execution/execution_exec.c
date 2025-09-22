#include "minishell.h"
#include "execution_utils.h"
#include <unistd.h> /* execve, _exit */
#include <stdlib.h> /* exit, free */
#include <stdio.h>  /* perror */

/* Has '/' anywhere in s? */
static int has_slash(char const *s)
{
    return (get_char_idx(s, '/') != (unsigned int)~0u);
}

/* Print "minishell: command not found: <cmd>\n" and exit 127. */
static void cmd_not_found(char const *cmd, char **envp)
{
    write(2, "minishell: command not found: ", 30);
    write(2, cmd, slen(cmd));
    write(2, "\n", 1);
    env_free_serialized(envp);
    _exit(127);
}

/* Try execve(path, argv, envp). On failure, perror(path) and exit 126. */
static void do_exec_or_fail(char const *path, char **argv, char **envp)
{
    execve(path, argv, envp);
    perror(path);
    env_free_serialized(envp);
    _exit(126);
}

void exec_command(t_command *cmd, t_env *env)
{
    char **envp;
    char *path;

    if (cmd->is_builtin)
        exit(exec_builtin(cmd->argv, env));
    set_child_signals();
    envp = env_serialize(env);
    if (!envp)
        _exit(1);
    path = find_command_path(cmd->argv, env);
    if (!path && has_slash(cmd->argv))
        do_exec_or_fail(cmd->argv, cmd->argv, envp);
    if (!path)
        cmd_not_found(cmd->argv, envp);
    do_exec_or_fail(path, cmd->argv, envp);
}
