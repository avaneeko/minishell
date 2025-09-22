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

// FIXME [MIN-35]: This is a placeholder. Please implement destroy_command properly.
// This does not belong here.
static void	Placeholder_DestroyCommand(t_command *cmd)
{
	free((void *)cmd->argv);
	free(cmd);
}

//
//	Creates a new command array. Initial capacity is set to CMDARR_INIT_CAP.
//	Returns 1 on success, 0 otherwise.
//
int		cmdarr_create(t_cmdarr *self)
{
	self->cmds = malloc(sizeof(t_command) * CMDARR_MEM_RESERVE);
	if (!self->cmds)
		return (0);
	self->cap = CMDARR_MEM_RESERVE;
	self->len = 0;
	return (1);
}

//
//	Destroys the command array, freeing all allocated memory.
//	Does not free the commands inside the array.
//	FIXME [MIN-34]: Make this deep free later.
//
void	cmdarr_destroy(t_cmdarr *self)
{
	free(self->cmds);
	mset(self, 0, sizeof(t_cmdarr));
}

//
//	Appends a command to the command array.
//	Will resize the command array if needed.
//	If free_on_fail is set to 1, the command will be freed using
//	Placeholder_DestroyCommand() on failure.
//	cmd is copied, but its contents are not deep-copied.
//	Returns 1 on success, 0 otherwise.
//
int	cmdarr_append(t_cmdarr *self, t_command const *cmd, int const free_on_fail)
{
	t_command	*new_cmds;

	// we cannot use realloc() so have to workaround with malloc() free().
	if (self->len >= self->cap)
	{
		new_cmds = malloc(sizeof(t_command) * (self->cap
					+ CMDARR_MEM_RESERVE));
		if (!new_cmds)
		{
			if (free_on_fail)
			{
				cmdarr_destroy(self);
				// Placeholder_DestroyCommand((t_command *)cmd);
			}
			return (0);
		}
		mcpy(new_cmds, self->cmds, sizeof(t_command) * self->len);
		free(self->cmds);
		self->cmds = new_cmds;
		self->cap += CMDARR_MEM_RESERVE;
	}
	mcpy(self->cmds + self->len++, cmd, sizeof(t_command));
	return (1);
}
