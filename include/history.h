#ifndef HISTORY_H
#define HISTORY_H

/**
 * add_to_history - Save a command to history
 * @command: The command string (e.g., "ls -la")
 * 
 * This function remembers what commands the user typed.
 */
void add_to_history(char *command);

/**
 * print_history - Show all commands in history
 */
void print_history(void);

/**
 * save_history - Save history to a file
 * File: ~/.mini_shell_history
 */
void save_history(void);

/**
 * load_history - Load history from file
 * File: ~/.mini_shell_history
 */
void load_history(void);

#endif
