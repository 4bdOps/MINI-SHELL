#include "../include/parser.h"
#include "../include/executor.h"
#include "../include/history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

/**
 * main - The main shell loop
 * 
 * 1. Print prompt
 * 2. Read user input
 * 3. Parse the command
 * 4. Execute the command
 * 5. Repeat until "exit"
 */
int main(void)
{
    char        input[BUFFER_SIZE];
    t_command   *cmd;
  

    // Load command history from file
    load_history();
    
    while (1)
    {
        // Print prompt
        printf("mini_shell$ ");
        fflush(stdout);
        
        // Read input from user
        if (!fgets(input, BUFFER_SIZE, stdin))
        {
            // EOF (Ctrl+D)
            printf("exit\n");
            break;
        }
        
        // Remove newline at the end
        input[strcspn(input, "\n")] = '\0';
        
        // Skip empty lines
        if (strlen(input) == 0)
            continue;
        
        // Add to history
        add_to_history(input);
        
        // Parse the command
        cmd = parse_command_line(input);
        if (!cmd)
        {
            fprintf(stderr, "mini_shell: Parse error\n");
            continue;
        }
        
        // Special case: exit command
        if (strcmp(cmd->name, "exit") == 0)
        {
            free_command(cmd);
            break;
        }
        
        // Execute the command
         execute_command(cmd);
        
        // Clean up
        free_command(cmd);
    }
    
    // Save history before exiting
    save_history();
    
    return 0;
}
