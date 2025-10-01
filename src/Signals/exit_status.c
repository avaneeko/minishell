
/* ************************************************************************** */
/*                                                                            */
/*                              exit_status.c                                  */
/*                                                                            */
/*   Overview                                                                  */
/*   - Minimal accessor pair for the shell’s exit status so signal handlers    */
/*     and the executor can update/read it consistently.                       */
/*   - Keep it simple and central; link this unit once in the build.           */
/*                                                                            */
/*   Note                                                                      */
/*   - Many 42 minishells use a global for this; if the local Norm policy      */
/*     forbids globals, wrap the storage behind these accessors and never      */
/*     expose it directly.                                                     */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

/* Define once in the program; handlers write through set_exit_status. */
static int  g_exit_status = 0;

int		get_exit_status(void)
{
	return (g_exit_status);
}

void	set_exit_status(int status)
{
	g_exit_status = status;
}
