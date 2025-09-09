#include "minishell.h"
#include "./execution_utils.h"
// #include <sys/wait.h>
// #include <unistd.h>
// #include <signal.h>
// #include <stdio.h>

/*
* Waits for a single child process to finish and sets global exit status.
* Returns pid of finished child or -1 on error.
*/
pid_t	wait_for_job(pid_t pid)
{
	int		status;
	pid_t	wpid;

	wpid = waitpid(pid, &status, 0);
	if (wpid == -1)
		return (-1);
	if (WIFEXITED(status))
		set_exit_status(WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		set_exit_status(128 + WTERMSIG(status));
	return (wpid);
}

/*
* Launch a command by forking and execve.
* Returns child's pid, or -1 on failure.
* cmd_path: full path to executable.
* argv: command arguments.
* envp: environment variables as array of strings.
*/
pid_t	launch_process(char *cmd_path, char **argv, char **envp)
{
	pid_t	pid;

	pid = fork();
	if (pid < 0)
	{
		perror("fork");
		return (-1);
	}
	if (pid == 0)
	{
		signal(SIGINT, SIG_DFL);
		signal(SIGQUIT, SIG_DFL);
		execve(cmd_path, argv, envp);
		perror("minishell: execve");
		_exit(127);
	}
	return (pid);
}

/*
* Runs a command and waits for its completion in foreground.
* Returns exit status of the command.
*/
int	run_command_fg(char *cmd_path, char **argv, char **envp)
{
	pid_t	pid;
	pid_t	wpid;
	int		status;

	pid = launch_process(cmd_path, argv, envp);
	if (pid < 0)
		return (1);
	while (1)
	{
		wpid = waitpid(pid, &status, 0);
		if (wpid == -1)
		{
			perror("waitpid");
			return (1);
		}
		if (wpid == pid)
			break ;
	}
	if (WIFEXITED(status))
		return (WEXITSTATUS(status));
	else if (WIFSIGNALED(status))
		return (128 + WTERMSIG(status));
	return (1);
}
