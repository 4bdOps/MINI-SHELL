

#include "../include/parser.h"
#include "../include/executor.h"
#include "../include/history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(void)
{
    char        input[BUFFER_SIZE];
    t_command   *cmd;

    while (1)
    {
        // Print prompt
        printf("mini_shell$ ");
        fflush(stdout);
        
        // Read input
        if (!fgets(input, BUFFER_SIZE, stdin))
        {
            printf("exit\n");
            break;
        }
        
        // Remove newline
        input[strcspn(input, "\n")] = '\0';
        
        // Skip empty lines
        if (strlen(input) == 0)
            continue;
        
        // Parse the command
        cmd = parse_command_line(input);
        if (!cmd)
        {
            fprintf(stderr, "mini_shell: Parse error\n");
            continue;
        }
        
        // Check for exit command
        if (strcmp(cmd->name, "exit") == 0)
        {
            free_command(cmd);
            break;
        }
        
        // For now, just show what we parsed
        printf("[PARSED] %s | Pipe: %s | Input file: %s | Output file: %s\n",
            cmd->name,
            cmd->is_pipe_output ? "YES" : "NO",
            cmd->input_file ? cmd->input_file : "NONE",
            cmd->output_file ? cmd->output_file : "NONE");
        
        // TODO: Execute the command
        
        // Cleanup
        free_command(cmd);
    }
    
    printf("\nGoodbye!\n");
    return 0;
}
