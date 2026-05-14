#include "../include/builtin.h"
#include "../include/error.h"
#include "../include/history.h"
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <errno.h>

#define MAX_PATH 1024

int is_builtin(char *name)
{
    if (!name)
        return 0;
    if (strcmp(name, "cd") == 0)    return 1;
    if (strcmp(name, "ls") == 0)    return 1;
    if (strcmp(name, "mkdir") == 0) return 1;
    if (strcmp(name, "pwd") == 0)   return 1;
    if (strcmp(name, "echo") == 0)  return 1;
    if (strcmp(name, "exit") == 0)  return 1;
    if (strcmp(name, "env") == 0)   return 1;
    if (strcmp(name, "setenv") == 0)   return 1;
    if (strcmp(name, "unsetenv") == 0) return 1;
    if (strcmp(name, "history") == 0)  return 1;
    return 0;
}

int execute_builtin(t_command *cmd)
{
    if (!cmd)
        return -1;

    if (strcmp(cmd->name, "cd") == 0)
        return cmd_cd(cmd->args, cmd->argc);
    if (strcmp(cmd->name, "ls") == 0)
        return cmd_ls(cmd->args, cmd->argc);
    if (strcmp(cmd->name, "mkdir") == 0)
        return cmd_mkdir(cmd->args, cmd->argc);
    if (strcmp(cmd->name, "pwd") == 0)
        return cmd_pwd(cmd->args, cmd->argc);
    if (strcmp(cmd->name, "echo") == 0)
        return cmd_echo(cmd->args, cmd->argc);
    if (strcmp(cmd->name, "exit") == 0)
        return cmd_exit(cmd->args, cmd->argc);
    if (strcmp(cmd->name, "env") == 0)
        return cmd_env(cmd->args, cmd->argc);
    if (strcmp(cmd->name, "setenv") == 0)
        return cmd_setenv(cmd->args, cmd->argc);
    if (strcmp(cmd->name, "unsetenv") == 0)
        return cmd_unsetenv(cmd->args, cmd->argc);
    if (strcmp(cmd->name, "history") == 0)
    {
        print_history();
        return 0;
    }

    print_error(cmd->name, "command not found");
    return -1;
}

/* ============================================================ */
/* IMPLEMENTATIONS                                              */
/* ============================================================ */

int cmd_cd(char **args, int argc)
{
    const char *target;

    (void)argc;
    target = (args[1] != NULL) ? args[1] : getenv("HOME");
    if (target == NULL)
    {
        print_error("cd", "HOME not set");
        return -1;
    }
    if (chdir(target) != 0)
    {
        print_perror("cd");
        return -1;
    }
    return 0;
}

int cmd_ls(char **args, int argc)
{
    const char  *target;
    DIR         *dir;
    struct dirent *entry;
    struct stat  st;
    char         fullpath[MAX_PATH];

    (void)argc;
    target = (args[1] != NULL) ? args[1] : ".";
    dir = opendir(target);
    if (dir == NULL)
    {
        print_perror("ls");
        return -1;
    }
    while ((entry = readdir(dir)) != NULL)
    {
        if (entry->d_name[0] == '.')
            continue;
        snprintf(fullpath, sizeof(fullpath), "%s/%s", target, entry->d_name);
        if (stat(fullpath, &st) == 0 && S_ISDIR(st.st_mode))
            printf("  [DIR]  %s\n", entry->d_name);
        else
            printf("         %s\n", entry->d_name);
    }
    closedir(dir);
    return 0;
}

int cmd_mkdir(char **args, int argc)
{
    (void)argc;
    if (args[1] == NULL)
    {
        print_error("mkdir", "missing operand");
        return -1;
    }
    if (mkdir(args[1], 0755) != 0)
    {
        print_perror("mkdir");
        return -1;
    }
    printf("Directory '%s' created.\n", args[1]);
    return 0;
}

int cmd_pwd(char **args, int argc)
{
    char cwd[MAX_PATH];

    (void)args;
    (void)argc;
    if (getcwd(cwd, sizeof(cwd)) == NULL)
    {
        print_perror("pwd");
        return -1;
    }
    printf("%s\n", cwd);
    return 0;
}

int cmd_echo(char **args, int argc)
{
    int i;

    for (i = 1; i < argc; i++)
    {
        if (i > 1)
            printf(" ");
        printf("%s", args[i]);
    }
    printf("\n");
    return 0;
}

int cmd_exit(char **args, int argc)
{
    (void)args;
    (void)argc;
    return 1;  /* Signal to main loop to exit */
}

int cmd_env(char **args, int argc)
{
    extern char **environ;
    int i;

    (void)args;
    (void)argc;
    for (i = 0; environ[i] != NULL; i++)
        printf("%s\n", environ[i]);
    return 0;
}

int cmd_setenv(char **args, int argc)
{
    (void)argc;
    if (args[1] == NULL || args[2] == NULL)
    {
        print_error("setenv", "usage: setenv NAME VALUE");
        return -1;
    }
    if (setenv(args[1], args[2], 1) != 0)
    {
        print_perror("setenv");
        return -1;
    }
    return 0;
}

int cmd_unsetenv(char **args, int argc)
{
    (void)argc;
    if (args[1] == NULL)
    {
        print_error("unsetenv", "missing variable name");
        return -1;
    }
    if (unsetenv(args[1]) != 0)
    {
        print_perror("unsetenv");
        return -1;
    }
    return 0;
}
