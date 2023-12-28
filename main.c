#include "main.h"

/**
 * main - The main function for the shell.
 *
 * Return: The exit status of the shell.
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
	}
	return (status);
}
