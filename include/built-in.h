#ifndef BUILTIN_H
#define BUILTIN_H

#include "parser.h"

/**
 * is_builtin - Check if a command is a built-in
 * @name: Command name (e.g., "cd", "echo")
 * Return: 1 if builtin, 0 if not
 */
int is_builtin(char *name);

/**
 * execute_builtin - Run a built-in command
 * @cmd: The parsed command
 * Return: 0 on success, -1 on error
 */
int execute_builtin(t_command *cmd);

/* ============ BUILT-IN FUNCTIONS ============ */

int cmd_cd(char **args, int argc);
int cmd_ls(char **args, int argc);
int cmd_mkdir(char **args, int argc);
int cmd_pwd(char **args, int argc);
int cmd_echo(char **args, int argc);
int cmd_exit(char **args, int argc);
int cmd_env(char **args, int argc);
int cmd_setenv(char **args, int argc);
int cmd_unsetenv(char **args, int argc);

#endif
