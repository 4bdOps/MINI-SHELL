#include "../include/executor.h"
#include "../include/builtin.h"
#include <stdio.h>

/**
 * execute_command - Execute a parsed command
 * 
 * TODO: P2, P3, P4 will implement this
 */
int execute_command(t_command *cmd)
{
    if (!cmd || !cmd->name)
    {
        fprintf(stderr, "Error: Invalid command\n");
        return -1;
    }
    
    // Check if it's a built-in command
    if (is_builtin(cmd->name))
        return execute_builtin(cmd);
    
    // TODO: External command execution
    fprintf(stderr, "mini_shell: %s: command not found\n", cmd->name);
    return -1;
}
