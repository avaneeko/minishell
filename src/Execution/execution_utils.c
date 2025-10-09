/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution_utils.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/09 20:00:41 by jgueon            #+#    #+#             */
/*   Updated: 2025/10/09 20:00:46 by jgueon           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "./execution_utils.h"

/**
 * @brief Counts the number of substrings in a string based on a delimiter
 *
 * This helper function counts how many words (substrings) will be created
 * when splitting the string with the given delimiter. It handles consecutive
 * delimiters by treating them as a single separator.
 *
 * @param s The string to analyze
 * @param c The delimiter character
 * @return The number of substrings that will be created
 */
static int	count_words(char const *s, char c)
{
	int	count;
	int	i;

	count = 0;
	i = 0;
	while (s[i] && s[i] == c)
		i++;
	while (s[i])
	{
		count++;
		while (s[i] && s[i] != c)
			i++;
		while (s[i] && s[i] == c)
			i++;
	}
	return (count);
}

/**
 * @brief Calculates the length of a word starting at a given position
 *
 * This helper function determines the length of a substring by counting
 * characters until it reaches either the delimiter or the end of the string.
 *
 * @param s The string containing the word
 * @param c The delimiter character
 * @param start The starting index of the word
 * @return The length of the word
 */
static int	word_length(char const *s, char c, int start)
{
	int	len;

	len = 0;
	while (s[start + len] && s[start + len] != c)
		len++;
	return (len);
}

/**
 * @brief Creates a substring from the input string
 *
 * This helper function extracts a word from the input string, allocates
 * memory for it, and copies the characters. It also updates the start
 * position for the next word.
 *
 * @param s The source string
 * @param c The delimiter character
 * @param start Pointer to the current position in the string
 * 		(updated by function)
 * @return A newly allocated string containing the word, or NULL if
 * 		allocation fails
 */
static char	*create_word(char const	*s, char c, int *start)
{
	int		len;
	char	*word;
	int		i;

	while (s[*start] && s[*start] == c)
		(*start)++;
	len = word_length(s, c, *start);
	word = (char *)malloc(sizeof(char) * (len + 1));
	if (!word)
		return (NULL);
	i = 0;
	while (i < len)
	{
		word[i] = s[*start + i];
		i++;
	}
	word[i] = '\0';
	*start += len;
	return (word);
}

/**
 * @brief Frees all allocated memory in case of failure
 *
 * This helper function is called when memory allocation fails during the
 * splitting process. It frees all previously allocated strings and the
 * array itself to prevent memory leaks.
 *
 * @param array The array of strings to free
 * @param size The number of strings already allocated in the array
 */
static void	free_array(char **array, int size)
{
	int	i;

	i = 0;
	while (i < size)
	{
		free(array[i]);
		array[i] = NULL;
		i++;
	}
	free(array);
	array = NULL;
}

/**
 * @brief Splits a string into an array of strings based on a delimiter
 *
 * This function splits the input string 's' into multiple substrings
 * wherever the character 'c' is encountered. The resulting array of strings
 * is NULL-terminated. Consecutive delimiters are treated as a single separator
 * Memory is properly managed with cleanup in case of allocation failure.
 *
 * @param s The string to be split
 * @param c The delimiter character
 * @return A NULL-terminated array of strings, or NULL if allocation fails or
 * 	s is NULL
 */
char	**ft_split(char const *s, char c)
{
	char	**result;
	int		word_count;
	int		i;
	int		start;

	if (!s)
		return (NULL);
	word_count = count_words(s, c);
	result = (char **)malloc(sizeof(char *) * (word_count + 1));
	if (!result)
		return (NULL);
	i = 0;
	start = 0;
	while (i < word_count)
	{
		result[i] = create_word(s, c, &start);
		if (!result[i])
		{
			free_array(result, i);
			return (NULL);
		}
		i++;
	}
	result[word_count] = NULL;
	return (result);
}
