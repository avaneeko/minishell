#include "minishell.h"

//$ Purpose: Declares command array for storing the execution plan.
// While t_cmdarr is an array and t_command is a linked list, t_cmdarr ensures
// that t_command's inside t_cmdarr->cmds form a valid linked list.

//
//	Initial memory allocation amount of t_command's on t_cmdarr creation.
//

typedef struct s_cmdarr
{
	t_command		*cmds;
	int unsigned	cap;
	int unsigned	len;
}	t_cmdarr;

int	cmdarr_create(t_cmdarr *self)
{
}

int	cmdarr_destroy(t_cmdarr *self)
{
}

int	cmdarr_append(t_cmdarr *self)
{
}
