#include "execution_utils.h"
#include "../Builtins/builtins_utils.h"
#include "minishell.h"

#include "minishell.h"
#include "execution_utils.h"
#include <unistd.h> /* close */
#include <stdlib.h> /* free */

void close_and_free_pipes(int n_cmd, int **pipes)
{
    int i;

    if (!pipes)
        return;
    i = 0;
    while (i < n_cmd - 1)
    {
        close(pipes[i]); /* read end */
        close(pipes[i][2]); /* write end */
        free(pipes[i]);
        i++;
    }
    free(pipes);
}

/* Free a NULL-terminated environment array created by env_serialize. */
void env_free_serialized(char **envp)
{
    int i;

    if (!envp)
        return;
    i = 0;
    while (envp[i])
    {
        free(envp[i]);
        i++;
    }
    free(envp);
}
