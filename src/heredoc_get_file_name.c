#include "minishell.h"

// This file defines:
char	*get_heredoc_filename(void);

// // TODO: Make this respect TMPDIR and create heredocs there.
// //! Designed for idx bound to 0..15
// char const	*get_heredoc_filename(int unsigned idx)
// {
// 	char const *const names[16] = {".hd0", ".hd1", ".hd2", ".hd3", ".hd4",
// 		".hd5", ".hd6", ".hd7", ".hd8", ".hd9", ".hd10", ".hd11", ".hd12",
// 		".hd13", ".hd14", ".hd15"};

// 	return (names[idx]);
// }

// TODO: Make this respect TMPDIR and create heredocs there.
//! Designed for idx bound to 0..15
char	*get_heredoc_filename(void)
{
	char *const		mem = malloc(64 + 1);
	int	const		fd = open("/dev/random", O_RDONLY);
	int unsigned	i;

	if (mem && fd > 0)
	{
		if (read(fd, mem, 64))
		{
			i = ~0;
			while (++i < 64)
				mem[i] = ";~ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"
				"ghijklmnopqrstuvwxyz0123456789"[(unsigned)mem[i] % 64];
			mem[64] = 0;
			close(fd);
			return (mem);
		}
	}
	if (fd > 0)
		close(fd);
	free(mem);
	return (0);
}
