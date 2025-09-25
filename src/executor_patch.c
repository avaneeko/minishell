


/* ************************************************************************** */
/*                                                                            */
/*   executor_patch.c                                                         */
/*                                                                            */
/*   Purpose:                                                                 */
/*   - Use the head (from redir_array_to_list) in prepare_fds_for_command.    */
/*   - This keeps builder unchanged while making executor consume a list.     */
/*                                                                            */
/*   Notes:                                                                   */
/*   - Original prepare_fds_for_command called setup_redirections(cmd->redirs)*/
/*     but the signature takes t_redir * (list), not t_redir ** (array) [2].  */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	prepare_fds_for_command(t_command *cmd)
{
	int		infd;
	int		outfd;
	t_redir	*head;

	infd = -1;
	outfd = -1;
	head = redir_array_to_list(cmd->redirs);
	if (setup_redirections(head, &infd, &outfd) == -1)
		return (-1);
	cmd->infile = infd;
	cmd->outfile = outfd;
	return (0);
}

/* ************************************************************************** */
/*                                                                            */
/*   Optional: free fix for builder side (when destroying a command)          */
/*                                                                            */
/*   - LEON.c has a TODO because t_command now holds t_redir ** [1].          */
/*   - Use free_redir_array when cleaning a command made by the builder.      */
/*                                                                            */
/* ************************************************************************** */

/* Example drop-in: replace redir free logic with free_redir_array(cmd->redirs) */
/* while ensuring cmd->redirs is set to NULL afterward [1].                     */
