#include "../include/parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* ========================================================== */
/* HELPER FUNCTIONS                                           */
/* ========================================================== */

/**
 * count_arguments - Count how many arguments in a string
 * @input: The input string (e.g., "ls -la /tmp")
 * Return: Number of words/arguments
 * 
 * Example: "ls -la /tmp" returns 3
 */
static int count_arguments(char *input)
{
    int     count;
    int     in_word;
    char    *ptr;
    
    if (!input || input[0] == '\0')
        return 0;
    
    count = 0;
    in_word = 0;
    ptr = input;
    
    // Walk through the entire string
    while (*ptr)
    {
        // If it's not a space
        if (*ptr != ' ' && *ptr != '\t')
        {
            // If we weren't in a word, start counting
            if (!in_word)
            {
                count++;
                in_word = 1;
            }
        }
        else
        {
            // Space found, mark that we're not in a word anymore
            in_word = 0;
        }
        ptr++;
    }
    
    return count;
}

/**
 * trim_input - Remove leading/trailing spaces
 * @input: The input string
 * Return: Pointer to trimmed string (inside original)
 * 
 * This helps us ignore spaces at the start/end of input
 */
static char *trim_input(char *input)
{
    while (*input && (*input == ' ' || *input == '\t'))
        input++;
    return input;
}

/* ========================================================== */
/* MAIN PARSER FUNCTIONS                                      */
/* ========================================================== */

/**
 * parse_command_line - Parse a command string into tokens
 * @input: The user input (e.g., "ls -la /tmp")
 * Return: A t_command struct, or NULL if error
 */
t_command *parse_command_line(char *input)
{
    t_command   *cmd;
    char        *input_copy;
    char        *token;
    int         argc;
    int         i;
    
    /* ===== INPUT VALIDATION ===== */
    if (!input || input[0] == '\0')
    {
        fprintf(stderr, "Error: Empty input\n");
        return NULL;
    }
    
    /* ===== ALLOCATE COMMAND STRUCTURE ===== */
    cmd = malloc(sizeof(t_command));
    if (!cmd)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        return NULL;
    }
    
    /* ===== INITIALIZE FIELDS ===== */
    cmd->name = NULL;
    cmd->args = NULL;
    cmd->argc = 0;
    cmd->input_file = NULL;
    cmd->output_file = NULL;
    cmd->append_output = 0;
    cmd->is_pipe_input = 0;
    cmd->is_pipe_output = 0;
    
    /* ===== TRIM INPUT ===== */
    input = trim_input(input);
    
    /* ===== COUNT ARGUMENTS ===== */
    argc = count_arguments(input);
    if (argc == 0)
    {
        fprintf(stderr, "Error: No command found\n");
        free(cmd);
        return NULL;
    }
    
    /* ===== ALLOCATE ARGS ARRAY ===== */
    // We need space for argc strings + 1 NULL terminator
    cmd->args = malloc(sizeof(char *) * (argc + 1));
    if (!cmd->args)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(cmd);
        return NULL;
    }
    
    /* ===== COPY INPUT (strtok modifies the string, so copy first) ===== */
    input_copy = malloc(strlen(input) + 1);
    if (!input_copy)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(cmd->args);
        free(cmd);
        return NULL;
    }
    strcpy(input_copy, input);
    
    /* ===== TOKENIZE ===== */
    i = 0;
    token = strtok(input_copy, " \t");  // Split by spaces and tabs
    
    while (token && i < argc)
    {
        // Allocate memory for this argument
        cmd->args[i] = malloc(strlen(token) + 1);
        if (!cmd->args[i])
        {
            fprintf(stderr, "Error: Memory allocation failed\n");
            free_command(cmd);
            free(input_copy);
            return NULL;
        }
        
        // Copy the token into args
        strcpy(cmd->args[i], token);
        
        // Move to next token
        token = strtok(NULL, " \t");
        i++;
    }
    
    // NULL terminate the args array (like argv in main)
    cmd->args[argc] = NULL;
    
    /* ===== SET COMMAND NAME AND ARGC ===== */
    cmd->name = cmd->args[0];  // First argument is the command name
    cmd->argc = argc;
    
    /* ===== CLEANUP ===== */
    free(input_copy);
    
    return cmd;
}

/**
 * free_command - Free all memory used by a command
 * @cmd: The command to free
 * 
 * IMPORTANT: Always call this after using a command!
 */
void free_command(t_command *cmd)
{
    int i;
    
    if (!cmd)
        return;
    
    // Free all arguments
    if (cmd->args)
    {
        i = 0;
        while (cmd->args[i])
        {
            free(cmd->args[i]);
            i++;
        }
        free(cmd->args);
    }
    
    // Free input/output files if they exist
    if (cmd->input_file)
        free(cmd->input_file);
    if (cmd->output_file)
        free(cmd->output_file);
    
    // Don't free cmd->name because it points to cmd->args[0]
    // which we already freed above
    
    // Free the command structure itself
    free(cmd);
}
