#ifndef PARSER_H
#define PARSER_H

// This is what a command looks like AFTER parsing
typedef struct {
    char    *name;          // "ls"
    char    **args;         // ["ls", "-la", "/tmp"]
    int     argc;           // 3
} Command;

// Function that does the parsing
Command *parse_command(char *input);

#endif
