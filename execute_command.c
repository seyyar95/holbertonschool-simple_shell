#include "main.h"

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
        {
            free(command);
            exit(EXIT_SUCCESS);
        }

        if (strcmp(args[0], "env") == 0)
        {
            char **env = environ;
            while (*env != NULL)
            {
                printf("%s\n", *env);
                env++;
            }
            free(command);
            exit(EXIT_SUCCESS);
        }

        if (strchr(args[0], '/') != NULL)
        {
            if (access(args[0], X_OK) == 0)
            {
                if (execve(args[0], args, environ) == -1)
                {
                    perror("execve");
                    fprintf(stderr, "Failed to execute %s\n", args[0]);
                    free(command);
                    exit(EXIT_FAILURE);
                }
            }
        }
        else
        {
            char *path = getenv("PATH");

            if (path == NULL)
            {
                fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
                free(command);
                exit(127);
            }

            char *token = strtok(path, ":");
            while (token != NULL)
            {
                char executable_path[256];
                int len = snprintf(executable_path, sizeof(executable_path), "%s/%s", token, args[0]);
                if (len >= sizeof(executable_path))
                {
                    fprintf(stderr, "Executable path truncated. Increase buffer size.\n");
                    free(command);
                    exit(EXIT_FAILURE);
                }

                if (access(executable_path, X_OK) == 0)
                {
                    if (execve(executable_path, args, environ) == -1)
                    {
                        perror("execve");
                        fprintf(stderr, "Failed to execute %s\n", args[0]);
                        free(command);
                        exit(EXIT_FAILURE);
                    }
                }

                token = strtok(NULL, ":");
            }
        }

        fprintf(stderr, "./hsh: 1: %s: not found\n", args[0]);
        free(command);
        exit(127);
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

    return status;
}
