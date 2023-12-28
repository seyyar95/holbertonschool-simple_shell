#include "main.h"

char *read_command(void)
{
    char *command = NULL;
    size_t bufsize = 0;

    ssize_t read_bytes = getline(&command, &bufsize, stdin);

    if (read_bytes == -1)
    {
        free(command);
        perror("getline");
        exit(EXIT_FAILURE);
    }

    if (command[read_bytes - 1] == '\n')
        command[read_bytes - 1] = '\0';

    return command;
}
