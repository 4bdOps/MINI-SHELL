#include "../include/parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* ========================================================== */
/* HELPER FUNCTIONS                                           */
/* ========================================================== */

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
    
    while (*ptr)
    {
        if (*ptr != ' ' && *ptr != '\t' && *ptr != '|' && 
            *ptr != '<' && *ptr != '>')
        {
            if (!in_word)
            {
                count++;
                in_word = 1;
            }
        }
        else
        {
            in_word = 0;
        }
        ptr++;
    }
    
    return count;
}

static char *trim_input(char *input)
{
    while (*input && (*input == ' ' || *input == '\t'))
        input++;
    return input;
}

/**
 * find_special_char - Find first occurrence of |, <, or >
 * @input: The input string
 * Return: Pointer to the special character, or NULL if none found
 */
static char *find_special_char(char *input)
{
    while (*input)
    {
        if (*input == '|' || *input == '<' || *input == '>')
            return input;
        input++;
    }
    return NULL;
}

/**
 * extract_filename - Extract filename after > or 
 * @input: Pointer to the > or < character
 * Return: Allocated string with filename, or NULL
 * 
 * Example: " > output.txt" returns "output.txt"
 */
static char *extract_filename(char *input)
{
    char    *start;
    char    *end;
    char    *filename;
    int     len;
    
    // Skip the > or < character
    input++;
    
    // Skip spaces
    while (*input && (*input == ' ' || *input == '\t'))
        input++;
    
    // If nothing after >, error
    if (!*input)
    {
        fprintf(stderr, "Error: No filename after redirection\n");
        return NULL;
    }
    
    start = input;
    
    // Find end of filename (next space or special char)
    while (*input && *input != ' ' && *input != '\t' && 
           *input != '|' && *input != '<' && *input != '>')
        input++;
    
    end = input;
    len = end - start;
    
    filename = malloc(len + 1);
    if (!filename)
        return NULL;
    
    strncpy(filename, start, len);
    filename[len] = '\0';
    
    return filename;
}

/**
 * parse_redirections - Find and extract < and > redirections
 * @input: The full input string
 * @cmd: The command to populate
 * Return: Input string with redirections removed
 * 
 * Side effect: Modifies cmd->input_file, cmd->output_file, etc.
 */
static char *parse_redirections(char *input, t_command *cmd)
{
    char    *cleaned_input;
    char    *ptr;
    char    *special;
    
    // Make a copy we can modify
    cleaned_input = malloc(strlen(input) + 1);
    if (!cleaned_input)
        return NULL;
    strcpy(cleaned_input, input);
    
    ptr = cleaned_input;
    
    // Look for > or 
    special = find_special_char(ptr);
    
    while (special)
    {
        if (*special == '>')
        {
            // Check if it's >> (append)
            if (*(special + 1) == '>')
            {
                cmd->output_file = extract_filename(special + 1);
                cmd->append_output = 1;
                
                // Remove >> and filename from string
                char *after = cmd->output_file ? 
                    special + 2 + strlen(cmd->output_file) : special + 2;
                
                // Copy the rest back
                strcpy(special, after);
                special = find_special_char(special);
            }
            else
            {
                cmd->output_file = extract_filename(special);
                cmd->append_output = 0;
                
                // Remove > and filename from string
                char *after = cmd->output_file ? 
                    special + 1 + strlen(cmd->output_file) : special + 1;
                
                strcpy(special, after);
                special = find_special_char(special);
            }
        }
        else if (*special == '<')
        {
            cmd->input_file = extract_filename(special);
            
            // Remove < and filename from string
            char *after = cmd->input_file ? 
                special + 1 + strlen(cmd->input_file) : special + 1;
            
            strcpy(special, after);
            special = find_special_char(special);
        }
        else
        {
            break;
        }
    }
    
    return cleaned_input;
}
/**
 * parse_pipes - Separate commands by pipes and mark them
 * @input: The full input string
 * Return: Array of t_command structures, or NULL if error
 * 
 * For now, we only handle the FIRST command before the pipe.
 * P4 will implement full pipeline execution.
 * 
 * Example:
 *   Input: "ls -la | grep .c"
 *   Returns: First command {name: "ls", is_pipe_output: 1}
 *   (The second command will be handled later)
 */
static char *find_pipe(char *input)
{
    while (*input)
    {
        // Skip quoted strings (don't count | inside quotes)
        if (*input == '"' || *input == '\'')
        {
            char quote = *input;
            input++;
            while (*input && *input != quote)
                input++;
            if (*input)
                input++;
        }
        else if (*input == '|')
        {
            return input;
        }
        else
        {
            input++;
        }
    }
    return NULL;
}

/**
 * extract_before_pipe - Get everything before the first pipe
 * @input: The full input string
 * Return: Allocated string with just the first command, or NULL
 * 
 * Example: "ls -la | grep .c" returns "ls -la"
 */
static char *extract_before_pipe(char *input)
{
    char    *pipe_pos;
    char    *result;
    int     len;
    
    pipe_pos = find_pipe(input);
    
    if (!pipe_pos)
    {
        // No pipe found, return the whole string
        result = malloc(strlen(input) + 1);
        if (result)
            strcpy(result, input);
        return result;
    }
    
    // Pipe found, extract everything before it
    len = pipe_pos - input;
    result = malloc(len + 1);
    if (!result)
        return NULL;
    
    strncpy(result, input, len);
    result[len] = '\0';
    
    return result;
}

/**
 * has_pipe - Check if input contains a pipe
 * @input: The input string
 * Return: 1 if pipe found, 0 otherwise
 */
static int has_pipe(char *input)
{
    return find_pipe(input) != NULL;
}
/**
 * is_quoted_char - Check if a position is inside quotes
 * @input: The string
 * @pos: Position to check
 * Return: 1 if position is inside quotes, 0 otherwise
 */
static int is_quoted_char(char *input, char *pos)
{
    int     in_single = 0;
    int     in_double = 0;
    char    *ptr = input;
    
    while (ptr < pos && *ptr)
    {
        if (*ptr == '\'' && !in_double)
            in_single = !in_single;
        else if (*ptr == '"' && !in_single)
            in_double = !in_double;
        ptr++;
    }
    
    return (in_single || in_double);
}

/**
 * count_arguments_quoted - Count arguments, respecting quotes
 * @input: The input string
 * Return: Number of arguments
 * 
 * Example: 'echo "hello world"' returns 2, not 3
 */
static int count_arguments_quoted(char *input)
{
    int     count;
    int     in_word;
    char    *ptr;
    
    if (!input || input[0] == '\0')
        return 0;
    
    count = 0;
    in_word = 0;
    ptr = input;
    
    while (*ptr)
    {
        // Check if current char is a space (but not inside quotes)
        int is_space = (*ptr == ' ' || *ptr == '\t');
        int is_special = (*ptr == '|' || *ptr == '<' || *ptr == '>');
        
        if (!is_space && !is_special)
        {
            if (!in_word)
            {
                count++;
                in_word = 1;
            }
            
            // Skip quoted strings
            if (*ptr == '"' || *ptr == '\'')
            {
                char quote = *ptr;
                ptr++;
                while (*ptr && *ptr != quote)
                    ptr++;
            }
        }
        else
        {
            in_word = 0;
        }
        
        if (*ptr)
            ptr++;
    }
    
    return count;
}

/**
 * tokenize_quoted - Split input into tokens, respecting quotes
 * @input: The input string (NOT modified)
 * @tokens: Array to store tokens (must be pre-allocated)
 * @argc: Number of tokens to extract
 * Return: 1 on success, 0 on error
 */
static int tokenize_quoted(char *input, char **tokens, int argc)
{
    char    *ptr;
    char    *token_start;
    int     in_quote;
    char    quote_char;
    int     token_idx;
    int     token_len;
    
    ptr = input;
    token_idx = 0;
    
    while (*ptr && token_idx < argc)
    {
        // Skip spaces
        while (*ptr && (*ptr == ' ' || *ptr == '\t'))
            ptr++;
        
        if (!*ptr)
            break;
        
        token_start = ptr;
        in_quote = 0;
        token_len = 0;
        
        // Extract one token (may include spaces if quoted)
        while (*ptr)
        {
            // Handle quotes
            if ((*ptr == '"' || *ptr == '\'') && !in_quote)
            {
                quote_char = *ptr;
                in_quote = 1;
                ptr++;
                continue;
            }
            
            if (*ptr == quote_char && in_quote)
            {
                in_quote = 0;
                ptr++;
                continue;
            }
            
            // End of token (space or pipe/redirect, but not in quotes)
            if (!in_quote && (*ptr == ' ' || *ptr == '\t' || 
                             *ptr == '|' || *ptr == '<' || *ptr == '>'))
                break;
            
            ptr++;
            token_len++;
        }
        
        // Allocate and copy token
        tokens[token_idx] = malloc(token_len + 1);
        if (!tokens[token_idx])
            return 0;
        
        // Copy token, removing quotes
        char *src = token_start;
        char *dst = tokens[token_idx];
        int idx = 0;
        
        while (src < ptr)
        {
            if (*src == '"' || *src == '\'')
            {
                src++;
                continue;
            }
            dst[idx] = *src;
            idx++;
            src++;
        }
        dst[idx] = '\0';
        
        token_idx++;
    }
    
    return 1;
}

/* ========================================================== */
/* MAIN PARSER FUNCTION                                       */
/* ========================================================== */

t_command *parse_command_line(char *input)
{
    t_command   *cmd;
    char        *input_copy;
    char        *cleaned_input;
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
    
    /* ===== PARSE REDIRECTIONS ===== */
    cleaned_input = parse_redirections(input, cmd);
    if (!cleaned_input)
    {
        free(cmd);
        return NULL;
    }
    
    /* ===== COUNT ARGUMENTS ===== */
    argc = count_arguments(cleaned_input);
    if (argc == 0)
    {
        fprintf(stderr, "Error: No command found\n");
        free(cleaned_input);
        free(cmd);
        return NULL;
    }
    
    /* ===== ALLOCATE ARGS ARRAY ===== */
    cmd->args = malloc(sizeof(char *) * (argc + 1));
    if (!cmd->args)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(cleaned_input);
        free(cmd);
        return NULL;
    }
    
    /* ===== COPY FOR TOKENIZATION ===== */
    input_copy = malloc(strlen(cleaned_input) + 1);
    if (!input_copy)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        free(cmd->args);
        free(cleaned_input);
        free(cmd);
        return NULL;
    }
    strcpy(input_copy, cleaned_input);
    
    /* ===== TOKENIZE ===== */
    i = 0;
    token = strtok(input_copy, " \t");
    
    while (token && i < argc)
    {
        cmd->args[i] = malloc(strlen(token) + 1);
        if (!cmd->args[i])
        {
            fprintf(stderr, "Error: Memory allocation failed\n");
            free_command(cmd);
            free(input_copy);
            free(cleaned_input);
            return NULL;
        }
        
        strcpy(cmd->args[i], token);
        token = strtok(NULL, " \t");
        i++;
    }
    
    cmd->args[argc] = NULL;
    
    /* ===== SET COMMAND NAME AND ARGC ===== */
    cmd->name = cmd->args[0];
    cmd->argc = argc;
    
    /* ===== CLEANUP ===== */
    free(input_copy);
    free(cleaned_input);
    
    return cmd;
}

void free_command(t_command *cmd)
{
    int i;
    
    if (!cmd)
        return;
    
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
    
    if (cmd->input_file)
        free(cmd->input_file);
    if (cmd->output_file)
        free(cmd->output_file);
    
    free(cmd);
}
