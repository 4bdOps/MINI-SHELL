#include "../include/scripts.h"
#include "../include/parser.h"
#include "../include/executor.h"
#include <stdio.h>
#include <string.h>

#define SCRIPT_BUF 1024

int execute_script(char *filename)
{
    FILE      *f;
    char       line[SCRIPT_BUF];
    t_command *cmd;

    f = fopen(filename, "r");
    if (f == NULL)
    {
        perror("script");
        return -1;
    }

    printf("Executing script '%s'...\n", filename);

    while (fgets(line, sizeof(line), f) != NULL)
    {
        /* Remove newline */
        line[strcspn(line, "\n")] = '\0';

        /* Skip empty lines and comments */
        if (line[0] == '\0' || line[0] == '#')
            continue;

        printf(">>> %s\n", line);

        cmd = parse_command_line(line);
        if (!cmd)
            continue;

        execute_command(cmd);
        free_command(cmd);
    }

    fclose(f);
    printf("Script done.\n");
    return 0;
}
