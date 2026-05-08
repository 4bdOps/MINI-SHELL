#include "../include/parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>

/* ========================================================== */
/* HELPER FUNCTIONS                                           */
/* ========================================================== */

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
                if (*ptr)
                    ptr++;
                continue;
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

/**
 * tokenize_quoted - Split input into tokens, handling quotes
 * @input: The input string
 * @tokens: Array to fill with token strings
 * @max_tokens: Maximum number of tokens to extract
 * Return: 1 on success, 0 on failure
 * 
 * Example: 'echo "hello world"' produces ["echo", "hello world"]
 */
static int tokenize_quoted(char *input, char **tokens, int max_tokens)
{
    int     token_idx;
    char    *ptr;
    
    if (!input || !tokens)
        return 0;
    
    token_idx = 0;
    ptr = input;
    
    while (*ptr && token_idx < max_tokens)
    {
        // Skip spaces
        while (*ptr && (*ptr == ' ' || *ptr == '\t'))
            ptr++;
        
        if (!*ptr)
            break;
        
        // Skip special characters (already handled by parse_redirections)
        if (*ptr == '|' || *ptr == '<' || *ptr == '>')
        {
            ptr++;
            continue;
        }
        
        // Start of a token
        char *token_start = ptr;
        int token_len = 0;
        int in_quote = 0;
        char quote_char = '\0';
        
        // Read until end of token
        while (*ptr)
        {
            // Handle quotes
            if ((*ptr == '"' || *ptr == '\'') && !in_quote)
            {
                quote_char = *ptr;
                in_quote = 1;
                ptr++;
                token_len++;
                continue;
            }
            
            if (*ptr == quote_char && in_quote)
            {
                in_quote = 0;
                ptr++;
                token_len++;
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
/**
 * check_quotes_balanced - Verify all quotes are closed
 * @input: The input string
 * Return: 1 if balanced, 0 if not
 */
static int check_quotes_balanced(char *input)
{
    int     in_single = 0;
    int     in_double = 0;
    
    while (*input)
    {
        if (*input == '\'' && !in_double)
            in_single = !in_single;
        else if (*input == '"' && !in_single)
            in_double = !in_double;
        
        input++;
    }
    
    // If still in a quote, it's unbalanced
    if (in_single || in_double)
    {
        fprintf(stderr, "Error: Unmatched quote\n");
        return 0;
    }
    
    return 1;
}

/**
 * check_redirections_valid - Verify redirections have filenames
 * @input: The input string
 * Return: 1 if valid, 0 if not
 */
static int check_redirections_valid(char *input)
{
    while (*input)
    {
        if (*input == '>' || *input == '<')
        {
            char    *ptr = input + 1;
            
            // Skip >> if it exists
            if (*ptr == '>')
                ptr++;
            
            // Skip spaces
            while (*ptr && (*ptr == ' ' || *ptr == '\t'))
                ptr++;
            
            // If we hit end of string, pipe, or another redirect, error
            if (!*ptr || *ptr == '|' || *ptr == '<' || *ptr == '>')
            {
                fprintf(stderr, "Error: Redirection operator without filename\n");
                return 0;
            }
        }
        
        input++;
    }
    
    return 1;
}

/**
 * check_pipes_valid - Verify pipes have commands on both sides
 * @input: The input string
 * Return: 1 if valid, 0 if not
 */
static int check_pipes_valid(char *input)
{
    char    *ptr;
    int     in_quote = 0;
    char    quote_char;
    
    ptr = input;
    
    while (*ptr)
    {
        // Track quotes
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
        
        // Check pipe outside quotes
        if (*ptr == '|' && !in_quote)
        {
            // Check something before pipe
            char *before = ptr - 1;
            while (before >= input && (*before == ' ' || *before == '\t'))
                before--;
            
            if (before < input || *before == '|')
            {
                fprintf(stderr, "Error: Pipe with no command before it\n");
                return 0;
            }
            
            // Check something after pipe
            char *after = ptr + 1;
            while (*after && (*after == ' ' || *after == '\t'))
                after++;
            
            if (!*after || *after == '|')
            {
                fprintf(stderr, "Error: Pipe with no command after it\n");
                return 0;
            }
        }
        
        ptr++;
    }
    
    return 1;
}

/**
 * sanitize_input - Clean and validate input
 * @input: The input string
 * Return: 1 if valid, 0 if not
 */
static int sanitize_input(char *input)
{
    // Check balanced quotes
    if (!check_quotes_balanced(input))
        return 0;
    
    // Check redirections are valid
    if (!check_redirections_valid(input))
        return 0;
    
    // Check pipes are valid
    if (!check_pipes_valid(input))
        return 0;
    
    return 1;
}

/* ========================================================== */
/* MAIN PARSER FUNCTION                                       */
/* ========================================================== */

t_command *parse_command_line(char *input)
{
    t_command   *cmd;
    char        *cleaned_input;
    char        *before_pipe;
    int         argc;
    
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
    
    /* ===== SANITIZE INPUT (NEW!) ===== */
    if (!sanitize_input(input))
    {
        free(cmd);
        return NULL;
    }
    
    /* ===== DETECT PIPES ===== */
    if (has_pipe(input))
    {
        before_pipe = extract_before_pipe(input);
        if (!before_pipe)
        {
            free(cmd);
            return NULL;
        }
        
        cmd->is_pipe_output = 1;
        input = before_pipe;
    }
    
    /* ===== PARSE REDIRECTIONS ===== */
    cleaned_input = parse_redirections(input, cmd);
    if (!cleaned_input)
    {
        int was_pipe = cmd->is_pipe_output;
        free(cmd);
        if (was_pipe)
            free((char *)input);
        return NULL;
    }
    
    /* ===== COUNT ARGUMENTS (WITH QUOTE HANDLING) ===== */
    argc = count_arguments_quoted(cleaned_input);
    if (argc == 0)
    {
        fprintf(stderr, "Error: No command found\n");
        int was_pipe = cmd->is_pipe_output;
        free(cleaned_input);
        if (was_pipe)
            free((char *)input);
        free(cmd);
        return NULL;
    }
    
    /* ===== ALLOCATE ARGS ARRAY ===== */
    cmd->args = malloc(sizeof(char *) * (argc + 1));
    if (!cmd->args)
    {
        fprintf(stderr, "Error: Memory allocation failed\n");
        int was_pipe = cmd->is_pipe_output;
        free(cleaned_input);
        if (was_pipe)
            free((char *)input);
        free(cmd);
        return NULL;
    }
    
    /* ===== TOKENIZE WITH QUOTE HANDLING ===== */
    if (!tokenize_quoted(cleaned_input, cmd->args, argc))
    {
        fprintf(stderr, "Error: Tokenization failed\n");
        int was_pipe = cmd->is_pipe_output;
        free(cmd->args);
        free(cleaned_input);
        if (was_pipe)
            free((char *)input);
        free(cmd);
        return NULL;
    }
    
    // NULL terminate the args array
    cmd->args[argc] = NULL;
    
    /* ===== SET COMMAND NAME AND ARGC ===== */
    cmd->name = cmd->args[0];
    cmd->argc = argc;
    
    /* ===== CLEANUP ===== */
    free(cleaned_input);
    if (cmd->is_pipe_output)
        free((char *)input);
    
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
