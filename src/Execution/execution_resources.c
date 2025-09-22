#include "minishell.h"
#include "execution_utils.h"
#include <unistd.h>  /* pipe, close */
#include <stdlib.h>  /* malloc, free */

/* Close/free pipes[0..made-1] and the outer array. */
static void free_pipes_partial(int **pipes, int made)
{
    int i;

    i = 0;
    while (i < made)
    {
        close(pipes[i]); /* read end */
        close(pipes[i][1]); /* write end */
        free(pipes[i]);
        i++;
    }
    free(pipes);
}

/* Allocate outer pipes array when needed; return 0 on success, -1 on error. */
static int alloc_pipes_outer(int n_cmd, int ***pipes_ptr)
{
    int **pipes;

    pipes = NULL;
    if (n_cmd > 1)
    {
        pipes = (int **)malloc(sizeof(int *) * (n_cmd - 1));
        if (!pipes)
            return (-1);
    }
    *pipes_ptr = pipes;
    return (0);
}

/* Allocate pid array; free pipes on error. Return 0 on success, -1 on error. */
static int alloc_pids_or_cleanup(int n_cmd, int **pipes, pid_t **pids_ptr)
{
    pid_t *pids;

    pids = (pid_t *)malloc(sizeof(pid_t) * n_cmd);
    if (!pids)
    {
        free(pipes);
        return (-1);
    }
    *pids_ptr = pids;
    return (0);
}

/* Allocate and open all pipe pairs; free on failure, return -1. */
static int alloc_open_pipes(int n_cmd, int **pipes, pid_t *pids)
{
    int i;

    i = 0;
    while (i < n_cmd - 1)
    {
        pipes[i] = (int *)malloc(sizeof(int) * 2);
        if (!pipes[i] || pipe(pipes[i]) == -1)
        {
            if (pipes[i])
                free(pipes[i]);
            free(pids);
            if (pipes)
                free_pipes_partial(pipes, i);
            return (-1);
        }
        i++;
    }
    return (0);
}

/* Allocate pids and N-1 pipes; return 0 on success, -1 on failure. */
int init_pipeline_resources(int n_cmd, int ***pipes_ptr, pid_t **pids_ptr)
{
    int   **pipes;
    pid_t *pids;

    if (alloc_pipes_outer(n_cmd, &pipes) == -1)
        return (-1);
    if (alloc_pids_or_cleanup(n_cmd, pipes, &pids) == -1)
        return (-1);
    if (alloc_open_pipes(n_cmd, pipes, pids) == -1)
        return (-1);
    *pipes_ptr = pipes;
    *pids_ptr = pids;
    return (0);
}
