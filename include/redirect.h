#ifndef REDIRECT_H
#define REDIRECT_H

#include "parser.h"

/**
 * handle_redirection - Set up input/output redirection
 * @cmd: The parsed command
 * 
 * Opens files and redirects stdin/stdout as needed.
 */
int handle_redirection(t_command *cmd);

/**
 * restore_redirection - Restore original stdin/stdout
 */
void restore_redirection(void);

int execute_pipeline_from_input(char *input,
    t_command *(*parse_fn)(char *),
    int (*execute_fn)(t_command *));

#endif
