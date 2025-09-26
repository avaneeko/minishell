#include "minishell.h"

//$ Purpose: Declares command array for storing the execution plan.
// While t_cmdarr is an array and t_command is a linked list, t_cmdarr ensures
// that t_command's inside t_cmdarr->cmds form a valid linked list.

/* Free argv vector and redirection list. */
// Helper function for MIN-35 to comply with Norm
// TODO: Fix this as t_command now has t_redir** instead of t_redir*
void free_command_payload(char **argv, t_redir *redirs)
{
	unsigned int i;
	t_redir *r;
	t_redir *next;

	i = 0;
	if (argv)
	{
		while (argv[i])
		{
			free(argv[i]);
			i++;
		}
		free(argv);
	}
	r = redirs;
	while (r)
	{
		next = r->next;
		if (r->type != TOKEN_HEREDOC) // Don't free the heredoc fd stored in target.
			free(r->target);
		free(r);
		r = next;
	}
}

// This does not belong here.
// Should be careful about cmd->infile and cmd->outfile not being stdin/out/err
// and accidentally closing them.
static void destroy_command(t_command *cmd)
{
	//free_command_payload(cmd->argv, cmd->redirs);
	if (cmd->infile != -1)
		close(cmd->infile);
	if (cmd->outfile != -1)
		close(cmd->outfile);
	cmd->argv = NULL;
	cmd->redirs = NULL;
	cmd->infile = -1;
	cmd->outfile = -1;
	cmd->is_builtin = 0;
	cmd->next = NULL;
}

//
//	Creates a new command array. Initial capacity is set to CMDARR_INIT_CAP.
//	Returns 1 on success, 0 otherwise.
//
int cmdarr_create(t_cmdarr *self)
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
//
void cmdarr_destroy(t_cmdarr *self)
{
	unsigned int	i;

	if (!self->cmds)
	{
		mset(self, 0, sizeof(*self));
		return ;
	}
	i = 0;
	while (i < self->len)
	{
		destroy_command(&self->cmds[i++]);
	}
	free(self->cmds);
	mset(self, 0, sizeof(*self));
}

//
//	Appends a command to the command array.
//	Will resize the command array if needed.
//	If destroy_on_fail is set to 1, the command will be freed using
//	destroy_command() and self with destroy_command() on failure.
//	cmd is copied, but its contents are not deep-copied.
//	Returns 1 on success, 0 otherwise.
//
int cmdarr_append(t_cmdarr *self, t_command *cmd, int const destroy_on_fail)
{
	t_command *new_cmds;

	if (self->len >= self->cap)
	{
		new_cmds = malloc(sizeof(t_command) * (self->cap + CMDARR_MEM_RESERVE));
		if (!new_cmds)
		{
			if (destroy_on_fail)
			{
				cmdarr_destroy(self);
				destroy_command(cmd);
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
