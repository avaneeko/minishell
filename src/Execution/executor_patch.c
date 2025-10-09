/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   executor_patch.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 21:09:00 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/09 21:16:39 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "execution_utils.h"

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
int	prepare_fds_for_command(t_app *app, t_command *cmd)
{
	int		infd;
	int		outfd;
	t_redir	*head;

	infd = -1;
	outfd = -1;
	if (cmd->redirs[0] == NULL)
		return (0);
	head = cmd->redirs[0];
	if (setup_redirections(app, head, &infd, &outfd) == -1)
		return (-1);
	cmd->infile = infd;
	cmd->outfile = outfd;
	return (0);
}
