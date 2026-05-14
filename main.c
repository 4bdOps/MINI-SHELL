#include "../include/parser.h"
#include "../include/executor.h"
#include "../include/history.h"
#include "../include/scripts.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

static void print_prompt(void)
{
    char cwd[BUFFER_SIZE];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
        printf("\033[1;32mmini_shell\033[0m:\033[1;34m%s\033[0m$ ", cwd);
    else
        printf("mini_shell$ ");
    fflush(stdout);
}

int main(void)
{
    char        input[BUFFER_SIZE];
    t_command  *cmd;

    printf("╔══════════════════════════════════════╗\n");
    printf("║         Mini-Shell v1.0              ║\n");
    printf("║  Type 'exit' to quit                 ║\n");
    printf("║  Type 'history' for history          ║\n");
    printf("╚══════════════════════════════════════╝\n\n");

    load_history();

    while (1)
    {
        print_prompt();

        if (!fgets(input, BUFFER_SIZE, stdin))
        {
            printf("\n");
            break;
        }

        /* Remove newline */
        input[strcspn(input, "\n")] = '\0';

        /* Skip empty lines */
        if (strlen(input) == 0)
            continue;

        /* Save to history */
        add_to_history(input);

        /* Handle 'source' script command */
        if (strncmp(input, "source ", 7) == 0)
        {
            execute_script(input + 7);
            continue;
        }

        /* Parse */
        cmd = parse_command_line(input);
        if (!cmd)
        {
            fprintf(stderr, "mini_shell: parse error\n");
            continue;
        }

        /* Execute (exit returns 1) */
        if (execute_command(cmd) == 1)
        {
            free_command(cmd);
            break;
        }

        free_command(cmd);
    }

    save_history();
    printf("\nGoodbye!\n");
    return 0;
}
