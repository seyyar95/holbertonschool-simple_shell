#include "main.h"

int env_builtin(char *command)
{
    return (strcmp(command, "env") == 0);
}

void print_environment()
{
    extern char **environ;
    char **env;
    for (env = environ; *env != NULL; env++)
    {
        printf("%s\n", *env);
    }
}
