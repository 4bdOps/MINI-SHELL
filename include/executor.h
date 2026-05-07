#ifndef EXECUTOR_H
#define EXECUTOR_H

#include "parser.h"

/**
 * execute_command - Run a command (builtin or external)
 * @cmd: The parsed command to execute
 * Return: 0 on success, -1 on error
 * 
 * This function decides:
 * - Is it a built-in command (cd, ls, etc.)?
 * - Or an external program?
 * Then executes it.
 */
int execute_command(t_command *cmd);

#endif
