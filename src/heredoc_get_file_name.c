#include "minishell.h"

// This file defines:
char const	*get_heredoc_filename(int unsigned idx);

// TODO: Make this respect TMPDIR and create heredocs there.
//! Designed for idx bound to 0..15
char const	*get_heredoc_filename(int unsigned idx)
{
	char const *const names[16] = {".hd0", ".hd1", ".hd2", ".hd3", ".hd4",
		".hd5", ".hd6", ".hd7", ".hd8", ".hd9", ".hd10", ".hd11", ".hd12",
		".hd13", ".hd14", ".hd15"};

	return (names[idx]);
}
