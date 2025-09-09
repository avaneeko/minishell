#include "minishell.h"
#include "./execution_utils.h"

static void	heredoc_sigint_handler(int signo)
{
	(void)signo;
	write(1, "\n", 1);
	exit(130);
}

static void	setup_heredoc_signals(void)
{
	signal(SIGINT, heredoc_sigint_handler);
	signal(SIGQUIT, SIG_IGN);
}

/*
* Reads lines from stdin until a line matching `delimiter` is entered.
* Writes all lines to write_fd, ends with closing the write end.
* Returns 0 on success, -1 on failure.
*/
int	heredoc_read(int write_fd, char *delimiter)
{
	char	*line;
	size_t	del_len;

	del_len = slen(delimiter);
	setup_heredoc_signals();
	while (1)
	{
		write(1, "> ", 2);
		line = readline(NULL);
		if (line == NULL)
			break ;
		if (ft_strncmp(line, delimiter, del_len) == 0 && line[del_len] == '\0')
		{
			free(line);
			break ;
		}
		write(write_fd, line, slen(line));
		write(write_fd, "\n", 1);
		free(line);
	}
	close(write_fd);
	signal(SIGINT, SIG_DFL); // Restore default signals after heredoc reading
	signal(SIGQUIT, SIG_DFL);
	return (0);
}

/*
* Creates a pipe and reads heredoc input with delimiter.
* Returns the read end fd on success, -1 on failure.
*/
int	open_heredoc(char *delimiter)
{
	int		fd[2];

	if (pipe(fd) == -1)
	{
		perror("pipe");
		return (-1);
	}
	if (fork() == 0)
	{
		close(fd[0]);
		if (heredoc_read(fd[1], delimiter) != 0)
			exit(1);
		exit(0);
	}
	close(fd[1]);
	wait(NULL);
	return (fd[0]);
}
