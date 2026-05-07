#ifndef ERROR_H
#define ERROR_H

/**
 * print_error - Print an error message consistently
 * @command: The command name
 * @message: The error message
 * 
 * Example: print_error("cd", "No such file or directory")
 * Output: mini_shell: cd: No such file or directory
 */
void print_error(char *command, char *message);

/**
 * print_perror - Print system error (from errno)
 * @command: The command name
 * 
 * Example: print_perror("cd")
 * Output: mini_shell: cd: No such file or directory (from system)
 */
void print_perror(char *command);

#endif
