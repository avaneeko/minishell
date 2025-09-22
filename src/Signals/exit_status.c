#include "minishell.h"

// Accessor/Mutator for Exit Status

int		get_exit_status(void)
{
	return (g_exit_status);
}

void	set_exit_status(int status)
{
	g_exit_status = status;
}
