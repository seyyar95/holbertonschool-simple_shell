

#include "main.h"

void execute_external_command(char **args);
void execute_command_child(char **args, char *command);
int execute_command(char *command);
void execute_env_command(void);
void execute_absolute_path_command(char **args);
void wait_and_free(char *command, int *status);


/**
 * read_command - reads command
 *
 * Return: string containing the command,
 * or NULL if an error occurs.
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
 * parse_arguments - parse arguments
 *
 *
 *@command: The command string to parse.
 *@args: An array of strings to store the parsed arguments.
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
 * execute_external_command - Execute an external command from given arguments
 * @args: Array of strings for command and its arguments
 *
 * Tries to execute an external command from PATH directories.
 * If found and executable, executes; otherwise, displays an error
 * and exits with status code 127 on failure.
 */

void execute_external_command(char **args)
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
				exit(EXIT_FAILURE);
			}
		}
		token = strtok(NULL, ":");
	}
	fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
	exit(127);
}

/**
 * execute_command_child - Execute command in a child process
 * @args: Array of strings for command and its arguments
 * @command: Command string
 *
 */
void execute_command_child(char **args, char *command)
{
	if (args[0] == NULL)
	{
		free(command);
		exit(EXIT_SUCCESS);
	}
	if (strcmp(args[0], "env") == 0)
		execute_env_command();
	if (strchr(args[0], '/') != NULL)
		execute_absolute_path_command(args);
	else
		execute_external_command(args);
}


/**
 * execute_command - Execute a command via forking process
 * @command: Command string to be executed
 *
 * Forks a process to execute a given command, waits for its
 * completion, and returns its status.
 * Return: Child process exit status or error code if fork fails.
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
		execute_command_child(args, command);
	}
	else
		 wait_and_free(command, &status);
	return (status);
}


/**
 * execute_env_command - Display environment variables
 *
 * Displays environment variables using a while loop
 * and exits with success status.
 */
void execute_env_command(void)
{
	char **env = environ;

	while (*env != NULL)
	{
		printf("%s\n", *env);
		env++;
	}
	exit(EXIT_SUCCESS);
}


/**
 * execute_absolute_path_command - Execute command by absolute path
 * @args: Array of strings containing command and its arguments
 *
 * Executes command by its absolute path if executable; otherwise,
 * executes as an external command.
 */

void execute_absolute_path_command(char **args)
{
	if (access(args[0], X_OK) == 0)
	{
		if (execve(args[0], args, environ) == -1)
		{
			perror("execve");
			exit(EXIT_FAILURE);
		}
	}
	execute_external_command(args);
}

/**
 * wait_and_free - Wait for child process and free command
 * @command: Command string to be freed
 * @status: Pointer to status of child process
 *
 * Waits for child process to complete, frees the command string,
 * and updates status based on child process exit status.
 */
void wait_and_free(char *command, int *status)
{
	waitpid(-1, status, 0);

	free(command);
	if (WIFEXITED(*status))
		*status = WEXITSTATUS(*status);
	else
		*status = 1;
}


/**
 * main - Entry poin
 *
 * Reads commands from the user and executes them.
 * Return: status
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
