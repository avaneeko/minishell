#include "minishell.h"
#include "builtins_utils.h"

// Find index of key in env or -1 if not found
int env_find_key(t_env *env, char *key)
{
    unsigned int i;

    i = 0;
    while (i < env->len)
    {
        if (key && env->pairs[i].key && streq(env->pairs[i].key, key))
            return (i);
        i++;
    }
    return (-1);
}

// Add or update key=value in env
int env_set(t_env *env, char *key, char *value, int origin)
{
    int idx;

    idx = env_find_key(env, key);
    if (idx >= 0)
    {
        free(env->pairs[idx].value);
        env->pairs[idx].value = ft_strdup(value);
        env->pairs[idx].origin = origin;
        return (1);
    }
    if (env->len >= env->cap)
        return (0);
    env->pairs[env->len].key = ft_strdup(key);
    env->pairs[env->len].value = ft_strdup(value);
    env->pairs[env->len].origin = origin;
    env->len++;
    return (1);
}

// Remove key from env
int env_unset(t_env *env, char *key)
{
    int idx;
    unsigned int i;

    idx = env_find_key(env, key);
    if (idx < 0)
        return (0);
    free(env->pairs[idx].key);
    free(env->pairs[idx].value);
    i = idx;
    while (i + 1 < env->len)
    {
        env->pairs[i] = env->pairs[i + 1];
        i++;
    }
    env->len--;
    return (1);
}

/**
 * This function converts the initial portion of the string pointed to by str
 * to an integer representation. It skips all white-space characters at the
 * beginning, takes an optional plus or minus sign followed by as many digits
 * as possible, and interprets them as a numerical value.
 * ft_atoi - Converts a string to an integer
 *
 * @param str: The string to be converted
 * @return The converted integer value
 */
int	ft_atoi(const char *str)
{
	int		sign;
	int		result;

	sign = 1;
	result = 0;
	while (*str == ' ' || (*str >= 9 && *str <= 13))
		str++;
	if (*str == '-' || *str == '+')
	{
		if (*str == '-')
			sign = -1;
		str++;
	}
	while (*str >= '0' && *str <= '9')
	{
		result = result * 10 + (*str - '0');
		str++;
	}
	return (result * sign);
}

/**
 * @brief Duplicates a string
 *
 * Description: This function allocates memory for a new string which is
 * a duplicate of the string s. Memory for the new string is obtained with
 * malloc, and can be freed with free.
 *
 * @param s: The string to duplicate
 *
 * @return A pointer to the duplicated string, or NULL if insufficient memory
 * was available
 */
char	*ft_strdup(const char *s1)
{
	char	*dup;
	int		len;
	int		i;

	len = 0;
	while (s1[len])
		len++;
	dup = (char *)malloc(sizeof(char) * (len + 1));
	if (dup == NULL)
		return (NULL);
	i = 0;
	while (i < len)
	{
		dup[i] = s1[i];
		i++;
	}
	dup[i] = '\0';
	return (dup);
}
