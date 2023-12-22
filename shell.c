#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>

/**
 * Reads a command from the user and returns it as a string.
 * Stores the command in memory through memory allocation.
 * Memory should be freed after usage.
 */

char *read_command(void)
{
	char *command = NULL;
	size_t bufsize = 0;

	ssize_t read_bytes = getline(&command, &bufsize, stdin);

	if (read_bytes == -1)
	{
		free(command);
		return (NULL);
	}
	if (command[read_bytes - 1] == '\n')
		command[read_bytes - 1] = '\0';

	return (command);
}

/**
 * Executes the given command in a new process.
 * Runs the command string using execve and waits for the process.
 * Handles memory deallocation.
 */

void exec_command(char *command)
{
	pid_t pid = fork();

	if (pid == -1)
	{
		perror("fork");
		free(command);
		exit(EXIT_FAILURE);
	}
	else if (pid == 0)
	{
		char *args[64];
		int arg_count = 0;
		char *token;

		token = strtok(command, " ");

		while (token != NULL && arg_count < 63)
		{
			args[arg_count++] = token;
			token = strtok(NULL, " ");
		}
		args[arg_count] = NULL;

		if (execve(args[0], args, NULL) == -1)
		{
			perror("execve");
			free(command);
			exit(EXIT_FAILURE);
		}
	}
	else
	{
		waitpid(pid, NULL, 0);
		free(command);
	}
}

/**
 * Displays a shell-like prompt for user input of a command.
 * Clears the stdout and takes command input from the user.
 * Calls read_command and exec_command functions to process the command.
 */

int main(void)
{
	int is_piped = !isatty(fileno(stdin));
	char *command;
	while (1)
	{
		if (!is_piped)
		{
			printf("#cisfun$ ");
			fflush(stdout);
		}

		command = read_command();

		if (command == NULL)
			break;

		exec_command(command);
	}
	return (0);
}
