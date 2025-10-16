/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_cd.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/23 19:01:30 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/16 00:44:45 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"

/**
 * @brief Decide the cd target: argv[1] if present, otherwise $HOME, printing
 * 		an error if HOME is unset.
 * @param env Environment to read HOME from.
 * @param argv Argument vector where argv[1] may be the target path.
 * @param out Output: set to chosen target string on success.
 * @return 1 on success, 0 if HOME is not set when no argument is provided.
 */
static int	resolve_target(t_env const *env, char **argv, char **out)
{
	t_epair	pair;

	*out = 0;
	if (argv && argv[1])
	{
		*out = argv[1];
		return (1);
	}
	if (!get_epair_by_key(env, "HOME", &pair))
	{
		write(2, "minishell: cd: HOME not set\n", 28);
		return (0);
	}
	*out = pair.value;
	return (1);
}

/**
 * @brief Fetch the current PWD value to be used as OLDPWD after a successful
 * 		cd.
 * @param env Environment to query for the "PWD" entry.
 * @return Pointer to the PWD value if present, otherwise NULL.
 */
static char	*fetch_oldpwd(t_env *env)
{
	t_epair	pair;

	if (get_epair_by_key(env, "PWD", &pair))
		return (pair.value);
	return (0);
}

/**
 * @brief Capture the current working directory into the provided buffer using getcwd.
 * @param buf Destination buffer to store the absolute path.
 * @param size Size of buf in bytes.
 * @return 1 on success, 0 on failure after printing an error message.
 */
static int	capture_cwd(char *buf, size_t size)
{
	if (!getcwd(buf, size))
	{
		perror("minishell: cd");
		return (0);
	}
	return (1);
}

/**
 * @brief Update OLDPWD and PWD in the environment after changing directory.
 * @param env Environment to modify.
 * @param oldpwd Previous working directory; treated as empty string if NULL.
 * @param cwd New current working directory to store in PWD.
 */
static void	update_pwd_vars(t_env *env, char *oldpwd, char *cwd)
{
	if (!oldpwd)
		oldpwd = "";
	env_set(env, "OLDPWD", oldpwd, ORIGIN_ENV);
	env_set(env, "PWD", cwd, ORIGIN_ENV);
}

/**
 * @brief Resolve 'cd' target from argv or $HOME, change directory, and
 * 		update PWD/OLDPWD.
 * @param argv Arguments where argv[1] may hold the target path.
 * @param env Environment to read HOME and update PWD/OLDPWD.
 * @return 0 on success, 1 on errors like too many args, missing HOME,
 * 		chdir failure, or getcwd failure.
 */
int	builtin_cd(char **argv, t_env *env)
{
	char	*target;
	char	cwd[PATH_MAX];
	char	*oldpwd;

	if (argv[1] && argv[2])
	{
		write(2, "minishell: cd: too many arguments\n", 34);
		return (1);
	}
	target = 0;
	if (!resolve_target(env, argv, &target))
		return (1);
	oldpwd = fetch_oldpwd(env);
	if (chdir(target) != 0)
	{
		perror("minishell: cd");
		return (1);
	}
	if (!capture_cwd(cwd, sizeof(cwd)))
		return (1);
	update_pwd_vars(env, oldpwd, cwd);
	return (0);
}
