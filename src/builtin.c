#include "../include/builtin.h"
#include "../include/error.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int is_builtin(char *name)
{
    if (!name)
        return 0;
    
    if (strcmp(name, "cd") == 0)
        return 1;
    if (strcmp(name, "ls") == 0)
        return 1;
    if (strcmp(name, "mkdir") == 0)
        return 1;
    if (strcmp(name, "pwd") == 0)
        return 1;
    if (strcmp(name, "echo") == 0)
        return 1;
    if (strcmp(name, "exit") == 0)
        return 1;
    if (strcmp(name, "env") == 0)
        return 1;
    if (strcmp(name, "setenv") == 0)
        return 1;
    if (strcmp(name, "unsetenv") == 0)
        return 1;
    
    return 0;
}

int execute_builtin(t_command *cmd)
{
    if (!cmd)
        return -1;
    
    // TODO: P2 will implement each of these
    printf("TODO: Implement builtin command: %s\n", cmd->name);
    return 0;
}

/* Placeholder implementations */

int cmd_cd(char **args, int argc)
{
    printf("TODO: cd\n");
    return 0;
}

int cmd_ls(char **args, int argc)
{
    printf("TODO: ls\n");
    return 0;
}

int cmd_mkdir(char **args, int argc)
{
    printf("TODO: mkdir\n");
    return 0;
}

int cmd_pwd(char **args, int argc)
{
    printf("TODO: pwd\n");
    return 0;
}

int cmd_echo(char **args, int argc)
{
    printf("TODO: echo\n");
    return 0;
}

int cmd_exit(char **args, int argc)
{
    return 1;  // Signal to exit
}

int cmd_env(char **args, int argc)
{
    printf("TODO: env\n");
    return 0;
}

int cmd_setenv(char **args, int argc)
{
    printf("TODO: setenv\n");
    return 0;
}

int cmd_unsetenv(char **args, int argc)
{
    printf("TODO: unsetenv\n");
    return 0;
}
