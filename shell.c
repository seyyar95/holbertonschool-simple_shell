#include "main.h"

#define MAX_ARGS 64
#define MAX_PATH_LENGTH 1024

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

void parse_arguments(char *command, char **args) {
    int arg_count = 0;
    char *token;
    const char *delimiters = " \t\n";
	char *end_quote;
	size_t len;

    token = strtok(command, delimiters);

    while (token != NULL && arg_count < MAX_ARGS - 1) {
         if (token[0] == '\"') {
            end_quote = strchr(token + 1, '\"');
            while (end_quote == NULL && arg_count < MAX_ARGS - 1) {
                args[arg_count] = token;
                arg_count++;

                token = strtok(NULL, delimiters);
                if (token == NULL) break;

                end_quote = strchr(token, '\"');
            }

            if (end_quote != NULL) {
                len = end_quote - token + 1;
                args[arg_count] = malloc(len);
                strncpy(args[arg_count], token, len - 1);
                args[arg_count][len - 1] = '\0';
                arg_count++;
            }
        } else {
            args[arg_count] = strdup(token);
            arg_count++;
            token = strtok(NULL, delimiters);
        }
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



int command_exists(const char *command) {
    if (strchr(command, '/') != NULL) {
         if (access(command, F_OK) == 0) {
            return 1;
        } else {
		  return 0;
        }
    }

    char *path_env = getenv("PATH");
    char *path_copy = strdup(path_env);

    if (path_copy == NULL) {
        perror("strdup");
        exit(EXIT_FAILURE);
    }

    char *path_dir;
    char full_path[MAX_PATH_LENGTH];

    path_dir = strtok(path_copy, ":");
    while (path_dir != NULL) {
        snprintf(full_path, sizeof(full_path), "%s/%s", path_dir, command);

        if (access(full_path, F_OK) == 0) {
            free(path_copy);
            return 1;
        }

        path_dir = strtok(NULL, ":");
    }

    free(path_copy);
    return 0;
}



void execute_command(char *command) {
    pid_t pid;
    char *args[MAX_ARGS];

    if (!command_exists(command)) {
        printf("%s: command not found\n", command);
        free(command);
        return;
    }

    pid = fork();

    if (pid == -1) {
        perror("fork");
        free(command);
        exit(EXIT_FAILURE);
    } else if (pid == 0) {
        parse_arguments(command, args);

        if (args[0] == NULL) {
            free(command);
            exit(EXIT_SUCCESS);
        }

        if (execvp(args[0], args) == -1) {
            perror("execvp");
            free(command);
            exit(EXIT_FAILURE);
        }
    } else {
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
