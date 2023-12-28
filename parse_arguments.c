#include "main.h"

/**
 * parse_arguments - Parses a command into individual arguments.
 *
 * @command: The command to be parsed.
 * @args: An array to store the parsed arguments.
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
