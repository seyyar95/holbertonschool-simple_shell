#include "main.h"

/**
 * read_command - Reads a command from the user and returns it as a string.
 * Return:
 *   Returns the command string or NULL on failure/EOF.
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
 * parse_arguments - Splits 'command' into separate arguments.
 * @command: Command string to be parsed.
 * @args: Array to store the extracted arguments.
 *
 * Uses strtok to tokenize 'command' by spaces into 'args'.
 * Limits the maximum number of arguments to 63.
 * Terminates 'args' array with NULL.
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

/**
 * execute_command - Executes a command and waits for its completion.
 * @command: Command string to execute.
 *
 * Creates a child process to execute 'command'.
 * Parses it into separate arguments using execve.
 * Parent waits for child to complete and frees allocated memory.
 */


void execute_command(char *command)
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

		parse_arguments(command, args);

		if (args[0] == NULL)
		{
			free(command);
			exit(EXIT_SUCCESS);
		}

		if (strcmp(args[0], "env") == 0)
		{
			char **env = environ;
			while(*env != NULL)
			{
				printf("%s\n", *env);
				env++;
			}
			free(command);
			exit(EXIT_SUCCESS);
		}
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
 * main - Basic shell interface for continuous command execution.
 *
 * Description:
 *   Prompts and executes user commands in a shell interface.
 *   Displays '#cisfun$ ' when not receiving input from a pipe,
 *   reads user input as commands, and executes them until exit/EOF.
 *
 * Return:
 *   Always returns 0 for successful completion.
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
		{
			break;
		}
		if (strcmp(command, "exit") == 0)
		{
			free(command);
			exit(0);
		}

		execute_command(command);
	}
	return (0);
}
