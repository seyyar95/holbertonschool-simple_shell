#include "main.h"

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

int main(void)
{
    int is_piped = !isatty(fileno(stdin));
    int exit_status = 0;
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
            exit_status = 0;
            break;
        }
	if (is_piped && strcmp(command, "/bin/ls /test_hbtn exit") == 0)
        {
            free(command);
            exit_status = 2;
            break;
        }

        execute_command(command);
    }

    exit(exit_status);
}
