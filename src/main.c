#include "../include/parser.h"
#include "../include/executor.h"
#include "../include/history.h"
#include "../include/redirect.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(void)
{
    char input[BUFFER_SIZE];

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

        // Check for exit command
        if (strcmp(input, "exit") == 0)
            break;
        
        // Execute the command (handles pipes, redirections, parsing)
        execute_pipeline_from_input(input, parse_command_line, execute_command);
    }
    
    printf("\nGoodbye!\n");
    return 0;
}