#include "../include/parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

/**
 * parse_command_line - Parse a command string
 * 
 * TODO: P1 (You) will implement this
 * 
 * For now, just return a basic example so it compiles
 */
t_command *parse_command_line(char *input)
{
    // TODO: Real implementation
    if (!input || input[0] == '\0')
        return NULL;
    
    t_command *cmd = malloc(sizeof(t_command));
    if (!cmd)
        return NULL;
    
    // Placeholder
    cmd->name = malloc(strlen(input) + 1);
    strcpy(cmd->name, input);
    cmd->args = NULL;
    cmd->argc = 0;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append_output = 0;
    cmd->is_pipe_input = 0;
    cmd->is_pipe_output = 0;
    
    return cmd;
}

void free_command(t_command *cmd)
{
    if (!cmd)
        return;
    
    // TODO: Real implementation
    if (cmd->name)
        free(cmd->name);
    if (cmd->args)
    {
        int i = 0;
        while (i < cmd->argc)
        {
            if (cmd->args[i])
                free(cmd->args[i]);
            i++;
        }
        free(cmd->args);
    }
    if (cmd->input_file)
        free(cmd->input_file);
    if (cmd->output_file)
        free(cmd->output_file);
    
    free(cmd);
}
