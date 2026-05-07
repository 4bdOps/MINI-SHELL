#ifndef PARSER_H
#define PARSER_H

#include <stddef.h>  // for NULL

/* ==================== COMMAND STRUCTURE ==================== */
/* This is what a "parsed command" looks like */

typedef struct s_command
{
    char    *name;              // "ls", "cd", "echo", etc.
    char    **args;             // ["ls", "-la", "/tmp"]
    int     argc;               // How many arguments (3 in above example)
    
    // For redirection (Day 4 will use these)
    char    *input_file;        // For: cat < input.txt
    char    *output_file;       // For: ls > output.txt
    int     append_output;      // 1 if >> (append), 0 if > (overwrite)
    
    // For pipes (Day 4 will use these)
    int     is_pipe_input;      // 1 if this command reads from a pipe
    int     is_pipe_output;     // 1 if this command writes to a pipe
}   t_command;

/* ==================== FUNCTIONS ==================== */

/**
 * parse_command_line - Parse a string into a t_command struct
 * @input: The user input (e.g., "ls -la /tmp")
 * Return: A pointer to t_command, or NULL if error
 * 
 * Example:
 *   t_command *cmd = parse_command_line("ls -la /tmp");
 *   // cmd->name = "ls"
 *   // cmd->args = ["ls", "-la", "/tmp"]
 *   // cmd->argc = 3
 */
t_command   *parse_command_line(char *input);

/**
 * free_command - Free all memory used by a command
 * @cmd: The command to free
 * 
 * IMPORTANT: Call this after you're done using a command!
 * Without this, memory leaks = bad.
 */
void        free_command(t_command *cmd);

#endif
