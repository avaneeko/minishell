/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_export.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 19:20:36 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/09 19:25:36 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"
#include "builtins_utils.h"
#include <unistd.h>  /* write */
#include <stdlib.h>

/*  - Behavior:                                                               */
/*      * No args: print sorted 'declare -x KEY="VALUE"' lines.               */
/*      * KEY: ensure a variable exists (empty value if new).                 */
/*      * KEY=VAL: set/replace.                                               */
/*      * KEY+=VAL: append VAL if key exists, else set.                       */
/*      * Invalid identifiers: print error, continue, final status 1 if any.  */
/*                                                                            */
/* ************************************************************************** */
/* ---------------------------- small I/O helpers --------------------------- */
/* print_str_fd: safe writer using slen from your utils.                      */
static void	print_str_fd(int fd, char const *s)
{
	if (s != NULL)
		write(fd, s, (int)slen(s));
}

/* print_export_error: matches minishell error style for invalid identifiers. */
static void	print_export_error(char const *arg)
{
	print_str_fd(2, "minishell: export: `");
	print_str_fd(2, arg);
	print_str_fd(2, "': not a valid identifier\n");
}

/* --------------------------- identifier utilities ------------------------- */
/* is_name_start: first char must be alpha or underscore.                     */
static int	is_name_start(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	if (c == '_')
		return (1);
	return (0);
}

/* is_name_char: subsequent chars may include digits.                         */
static int	is_name_char(char c)
{
	if ((c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z'))
		return (1);
	if (c == '_' || (c >= '0' && c <= '9'))
		return (1);
	return (0);
}

/* scan_key_body: walk past a valid NAME, stop at '=' or '+=' or end. */
static int	scan_key_body(char const *arg, int start)
{
	int	i;

	i = start;
	while (arg[i] && arg[i] != '=' && !(arg[i] == '+' && arg[i + 1] == '='))
	{
		if (!is_name_char(arg[i]))
			return (-1);
		i += 1;
	}
	return (i);
}

/* parse_key_mode: now short; sets key_len, is_append, has_eq or fails. */
static int	parse_key_mode(char const *arg, int *key_len, int *is_append,
		int *has_eq)
{
	int	i;

	*is_append = 0;
	*has_eq = 0;
	if (!arg || !arg[0] || !is_name_start(arg[0]))
		return (0);
	i = scan_key_body(arg, 1);
	if (i < 0)
		return (0);
	if (arg[i] == '=')
	{
		*key_len = i;
		*has_eq = 1;
		return (1);
	}
	if (arg[i] == '+' && arg[i + 1] == '=')
	{
		*key_len = i;
		*is_append = 1;
		*has_eq = 1;
		return (1);
	}
	*key_len = i;
	return (1);
}

/* ------------------------------ string helpers ---------------------------- */
/* strlen_or_zero: guards against NULL when building strings.                 */
static size_t	strlen_or_zero(char const *s)
{
	if (s == NULL)
		return (0);
	return (slen(s));
}

/* copy_bytes: append src bytes to dst at index pointer.                      */
static void	copy_bytes(char *dst, size_t *idx, char const *src)
{
	size_t	i;

	i = 0;
	if (!src)
		return ;
	while (src[i])
	{
		dst[*idx + i] = src[i];
		i += 1;
	}
	*idx = *idx + i;
}

/* join_kv: allocate "KEY=VALUE" string for create_pair.                      */
static char	*join_kv(char const *k, char const *v)
{
	size_t	kl;
	size_t	vl;
	char	*s;
	size_t	i;

	kl = slen(k);
	vl = strlen_or_zero(v);
	s = (char *)malloc(kl + 1 + vl + 1);
	if (!s)
		return (0);
	i = 0;
	copy_bytes(s, &i, k);
	s[i] = '=';
	i += 1;
	copy_bytes(s, &i, v);
	s[i] = 0;
	return (s);
}

/* --------------------------- environment updates -------------------------- */
/* set_env_replace: remove existing key and set to value.                     */
static int	set_env_replace(t_env *env, char const *key, char const *val)
{
	t_epair	pair;
	char	*kv;
	int		ok;

	remove_epair_by_key(env, key);
	kv = join_kv(key, val);
	if (!kv)
		return (0);
	ok = create_pair(kv, &pair);
	free(kv);
	if (!ok)
		return (0);
	if (!try_append_epair(env, &pair))
		return (0);
	return (1);
}

/* set_env_append: key+=val or fallback to replace if key missing.            */
static int	set_env_append(t_env *env, char const *key, char const *val)
{
	t_epair	cur;
	char	*merged;
	size_t	len;
	size_t	idx;

	if (!get_epair_by_key(env, key, &cur))
		return (set_env_replace(env, key, val));
	len = slen(cur.value) + slen(val);
	merged = (char *)malloc(len + 1);
	if (!merged)
		return (0);
	idx = 0;
	copy_bytes(merged, &idx, cur.value);
	copy_bytes(merged, &idx, val);
	merged[idx] = 0;
	if (!set_env_replace(env, key, merged))
	{
		free(merged);
		return (0);
	}
	free(merged);
	return (1);
}

/* exists_key: returns 1 if key is present in env.                            */
static int	exists_key(t_env *env, char const *key)
{
	t_epair	tmp;

	if (get_epair_by_key(env, key, &tmp))
		return (1);
	return (0);
}

/* ------------------------------- printing path ---------------------------- */
/* count_strv: count serialized env entries.                                  */
static int	count_strv(char **v)
{
	int	n;

	n = 0;
	if (!v)
		return (0);
	while (v[n])
		n += 1;
	return (n);
}

/* cmp_str: lexicographic compare for sort.                                   */
static int	cmp_str(char const *a, char const *b)
{
	size_t	i;

	i = 0;
	while (a[i] && b[i] && a[i] == b[i])
		i += 1;
	return ((unsigned char)a[i] - (unsigned char)b[i]);
}

/* swap_ptr: swap two pointers for bubble sort.                               */
static void	swap_ptr(char **a, char **b)
{
	char	*t;

	t = *a;
	*a = *b;
	*b = t;
}

/* bubble_one_pass: one pass of bubble sort and report if swapped.            */
static int	bubble_one_pass(char **v, int n)
{
	int	j;
	int	swapped;

	if (!v || n <= 1)
		return (0);
	j = 0;
	swapped = 0;
	while (j + 1 < n)
	{
		if (cmp_str(v[j], v[j + 1]) > 0)
		{
			swap_ptr(&v[j], &v[j + 1]);
			swapped = 1;
		}
		j += 1;
	}
	return (swapped);
}

/* sort_lex: bubble until no swaps to keep code simple and Norm-compliant.    */
static void	sort_lex(char **v, int n)
{
	int	changed;

	changed = 1;
	while (changed)
	{
		changed = bubble_one_pass(v, n);
	}
}

/* print_one_decl: prints 'declare -x KEY="VALUE"' for a serialized entry.    */
static void	print_one_decl(char const *kv)
{
	int	i;

	print_str_fd(1, "declare -x ");
	i = 0;
	while (kv[i] && kv[i] != '=')
	{
		write(1, &kv[i], 1);
		i += 1;
	}
	if (kv[i] == '=')
	{
		print_str_fd(1, "=\"");
		i += 1;
		while (kv[i])
		{
			write(1, &kv[i], 1);
			i += 1;
		}
		print_str_fd(1, "\"");
	}
	print_str_fd(1, "\n");
}

/* print_export_noargs: serialize, sort, and print all variables.             */
static int	print_export_noargs(t_env const *env)
{
	char	**envp;
	int		n;
	int		i;

	envp = env_serialize(env);
	if (!envp)
		return (0);
	n = count_strv(envp);
	sort_lex(envp, n);
	i = 0;
	while (i < n)
	{
		print_one_decl(envp[i]);
		i += 1;
	}
	env_free_serialized(envp);
	return (1);
}

/* ------------------------------- handlers --------------------------------- */
/* handle_export_noeq: ensure KEY exists; create empty if missing.            */
static int	handle_export_noeq(t_env *env, char const *key)
{
	if (!exists_key(env, key))
	{
		if (!set_env_replace(env, key, ""))
			return (1);
	}
	return (0);
}

/* alloc_key_copy: make a heap copy of KEY substring (length key_len).        */
static char	*alloc_key_copy(char const *arg, int key_len)
{
	char	*key;
	int		i;

	key = (char *)malloc((size_t)key_len + 1);
	if (!key)
		return (0);
	i = 0;
	while (i < key_len)
	{
		key[i] = arg[i];
		i += 1;
	}
	key[key_len] = 0;
	return (key);
}

/* val_ptr_after_key: compute pointer to value after '=' or '+='.             */
static char const	*val_ptr_after_key(char const *arg, int key_len,
				int is_append)
{
	int	off;

	off = 1;
	if (is_append)
		off = 2;
	return (arg + key_len + off);
}

/* apply_set_or_append: call append or replace and map to 0/1 return.         */
static int	apply_set_or_append(t_env *env, char const *key,
		char const *val, int is_append)
{
	if (is_append)
	{
		if (!set_env_append(env, key, val))
			return (1);
		return (0);
	}
	if (!set_env_replace(env, key, val))
		return (1);
	return (0);
}

/* handle_export_with_eq: split, then set or append; now ≤25 lines.           */
static int	handle_export_with_eq(t_env *env, char const *arg,
		int key_len, int is_append)
{
	char		*key;
	char const	*val;
	int			ret;

	key = alloc_key_copy(arg, key_len);
	if (!key)
		return (1);
	val = val_ptr_after_key(arg, key_len, is_append);
	ret = apply_set_or_append(env, key, val, is_append);
	free(key);
	return (ret);
}

/* process_one_export_arg: validate and dispatch no-eq vs with-eq             */
static int	process_one_export_arg(t_env *env, char const *arg)
{
	int	key_len;
	int	is_append;
	int	has_eq;

	if (!parse_key_mode(arg, &key_len, &is_append, &has_eq))
	{
		print_export_error(arg);
		return (1);
	}
	if (!has_eq)
		return (handle_export_noeq(env, arg));
	return (handle_export_with_eq(env, arg, key_len, is_append));
}

int	builtin_export(char **argv, t_env *env)
{
	int	i;
	int	status;

	if (!argv || !argv[0] || !env)
		return (0);
	if (!argv[1])
	{
		if (print_export_noargs(env))
			return (0);
		return (1);
	}
	i = 1;
	status = 0;
	while (argv[i])
	{
		if (process_one_export_arg(env, argv[i]) != 0)
			status = 1;
		i += 1;
	}
	return (status);
}
