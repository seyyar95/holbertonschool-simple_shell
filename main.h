#ifndef MAIN_H
#define MAIN_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <string.h>
extern char **environ;
char *read_command(void);
void parse_arguments(char *command, char **args);
int execute_command(char *command);
int execute_builtin(char **args);
int execute_absolute_path(char **args);
int execute_from_path(char **args);
int main(void);


#endif
