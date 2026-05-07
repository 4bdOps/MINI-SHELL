#include "../include/error.h"
#include <stdio.h>
#include <errno.h>
#include <string.h>

// TODO: P3 will implement these

void print_error(char *command, char *message)
{
    fprintf(stderr, "mini_shell: %s: %s\n", command, message);
}

void print_perror(char *command)
{
    fprintf(stderr, "mini_shell: %s: %s\n", command, strerror(errno));
}
