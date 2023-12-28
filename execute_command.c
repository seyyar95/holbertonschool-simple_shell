#include "main.h"

/**
 * execute_command - Executes a given command.
 *
 * @command: The command to be executed.
 *
 * Return: The exit status of the executed command.
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
			exit(EXIT_SUCCESS);
		if (execute_builtin(args) == 0)
			exit(EXIT_SUCCESS);
		if (execute_absolute_path(args) == 0)
			exit(EXIT_SUCCESS);
		if (execute_from_path(args) == 0)
			exit(EXIT_SUCCESS);

		fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
		free(command);
		exit(2);
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

/**
 * execute_builtin - Executes built-in commands.
 *
 * @args: The command arguments.
 *
 * Return: 1 if the command is not a built-in, 0 otherwise.
 */
int execute_builtin(char **args)
{
	if (strcmp(args[0], "env") == 0)
	{
		char **env = environ;

		while (*env != NULL)
		{
			printf("%s\n", *env);
			env++;
		}
		return (0);
	}
	return (1);
}

/**
 * execute_from_path - Executes a command from the PATH environment variable.
 *
 * @args: The command arguments.
 *
 * Return: 1 if the command is not found in PATH, 0 otherwise.
 */
int execute_from_path(char **args)
{
	char *path = getenv("PATH");
	char *token;

	if (path == NULL)
	{
		fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
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
				free(args[0]);
				free(args);
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, ":");
	}
	return (1);
}

/**
 * execute_absolute_path - Executes a command with an absolute path.
 *
 * @args: The command arguments.
 *
 * Return: 1 if the command is not an absolute path, 0 otherwise.
 */
int execute_absolute_path(char **args)
{
	if (strchr(args[0], '/') != NULL)
	{
		if (access(args[0], X_OK) == 0)
		{
			if (execve(args[0], args, environ) == -1)
			{
				perror("execve");
				free(args[0]);
				free(args);
				exit(EXIT_FAILURE);
			}
		}
		return (0);
	}
	return (1);
}
