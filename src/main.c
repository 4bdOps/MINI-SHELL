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

    // Load history (P3 will implement this)
    // load_history();
    
    while (1)
    {
        // Print prompt
        printf("mini_shell$ ");
        fflush(stdout);
        
        // Read input
        if (!fgets(input, BUFFER_SIZE, stdin))
        {
            // EOF (Ctrl+D)
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
        
        // Print what we parsed (for debugging)
        printf("[DEBUG] Parsed command: %s with %d args\n", cmd->name, cmd->argc);
        
        // Add to history (P3 will implement)
        // add_to_history(input);
        
        // Check for exit command
        if (strcmp(cmd->name, "exit") == 0)
        {
            free_command(cmd);
            break;
        }
        
        // Execute the command (P2/P3/P4 will implement)
        // execute_command(cmd);
        
        // For now, just print it
        printf("→ Would execute: %s\n", cmd->name);
        
        // Cleanup
        free_command(cmd);
    }
    
    // Save history
    // save_history();
    
    printf("\nGoodbye!\n");
    return 0;
}
