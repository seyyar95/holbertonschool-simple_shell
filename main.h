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

char *read_command(void);
void parse_arguments(char *command, char **args);
void execute_command(char *command);
int main(void);

#endif
