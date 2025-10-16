/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   expand_last_err.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/14 16:35:50 by losypenk          #+#    #+#             */
/*   Updated: 2025/10/14 16:36:21 by losypenk         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

//$ Purpose: Expand the special variable $? to the last command's exit status.
//$ This is done here because it's a special case of variable expansion.

// This file implements:
int	expand_last_err(t_app *app);

int	expand_last_err(t_app *app)
{
	int const	status = app->last_exit_code & 0xFF;
	char		*buf;

	buf = app->last_exit_code_str;
	if (status < 10)
	{
		buf[0] = '0' + status;
		buf[1] = 0;
	}
	else if (status < 100)
	{
		buf[0] = '0' + (status / 10);
		buf[1] = '0' + (status % 10);
		buf[2] = 0;
	}
	else
	{
		buf[0] = '0' + (status / 100);
		buf[1] = '0' + ((status / 10) % 10);
		buf[2] = '0' + (status % 10);
		buf[3] = 0;
	}
	return (1);
}
