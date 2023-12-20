#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

/**
 * main - Simple UNIX command line interpreter
 *
 * Return: Always 0.
 */
int main(void)
{
    char *line = NULL;
    size_t len = 0;
    ssize_t read;
    pid_t child_pid;
    int status;

    while (1)
    {
        printf("$ ");
        read = getline(&line, &len, stdin);
        if (read == -1)
            break;
        if (line[read - 1] == '\n')
            line[read - 1] = '\0';
        child_pid = fork();
        if (child_pid == -1)
        {
            perror("Error");
            return (1);
        }
        if (child_pid == 0)
        {
			char *args[] = {line, NULL};
    		if (execve(line, args, NULL) == -1)
    		{
        		perror("Error");
        		return (1);
    		}
        }
        else
            wait(&status);
    }
    free(line);
    exit(EXIT_SUCCESS);
}
