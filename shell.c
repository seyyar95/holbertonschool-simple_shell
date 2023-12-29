
#include "main.h"

/*
 * read_command - Reads a command from the user's input.
 *
 * Returns:
 * - A string containing the command, or NULL if an error occurs.
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

/*
 * parse_arguments - Parses a command string into individual arguments.
 *
 * Arguments:
 * - command: The command string to parse.
 * -  args: An array of strings to store the parsed arguments.
 */

void parse_arguments(char *command, char **args)
{
	int arg_count = 0;
	char *token;

	token = strtok(command, " ");

	while (token != NULL && arg_count < 63)
	{
		args[arg_count++] = token;
		token = strtok(NULL, " ");
	}
	args[arg_count] = NULL;
}

/*
 * execute_command - Executes a given command.
 *
 * Arguments:
 *  - command: The command string to execute.
 *
 * Returns:
 *  - The exit status of the executed command, or -1 if an error occurs.
 */

int execute_command(char *command)
{
	int status = 0;
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

		parse_arguments(command, args);
		if (args[0] == NULL)
		{
			free(command);
			exit(EXIT_SUCCESS);
		}
		if (strcmp(args[0], "env") == 0)
		{
			char **env = environ;

			while (*env != NULL)
			{
				printf("%s\n", *env);
				env++;
			}
			free(command);
			exit(EXIT_SUCCESS);
		}
		if (strchr(args[0], '/') != NULL)
		{
			if (access(args[0], X_OK) == 0)
			{
				if (execve(args[0], args, environ) == -1)
				{
					perror("execve");
					free(command);
					exit(EXIT_FAILURE);
				}
			}
		}
		else
		{
			char *path = getenv("PATH");
			char *token;

			if (path == NULL)
			{
				fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
				free(command);
				exit(127);
			}
			token = strtok(path, ":");

			while (token != NULL)
			{
				char executable_path[256];

				snprintf(executable_path, sizeof(executable_path), "%s/%s", token, args[0]);
				if (access(executable_path, X_OK) == 0)
				{
					if (execve(executable_path, args, environ) == -1)
					{
						perror("execve");
						free(command);
						exit(EXIT_FAILURE);
					}
				}

				token = strtok(NULL, ":");
			}
		}
		fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
		free(command);
		exit(127);
	}
	else
	{
		waitpid(pid, &status, 0);
		free(command);
		if (WIFEXITED(status))
			status = WEXITSTATUS(status);
		else
			status = 1;
	}
	return (status);
}

/*
 * main - Main function of the shell.
 *
 * Reads commands from the user and executes them.
 * Returns: status
 */

int main(void)
{
	int is_piped = !isatty(fileno(stdin));
	char *command;
	int status = 0;

	while (1)
	{
		if (!is_piped)
		{
			printf("#cisfun$ ");
			fflush(stdout);
		}
		command = read_command();
		if (command == NULL)
		{
			break;
		}
		if (strcmp(command, "exit") == 0)
		{
			free(command);
			exit(0);
		}
		status = execute_command(command);
		if (status == 2 && is_piped)
		{
			exit(2);
		}
	}
	return (status);
}
