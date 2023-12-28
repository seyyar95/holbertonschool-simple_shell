#include "main.h"
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

