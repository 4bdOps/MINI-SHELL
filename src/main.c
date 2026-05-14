#include "minishell.h"

/* Global history (shared with builtins) */
History g_history;

/* Commands that MUST run in the parent process (they alter shell state) */
static int must_run_in_parent(const char *name)
{
    static const char *plist[] = {
        "cd", "exit", "export", "unset", "history", "help", "clear", NULL
    };
    for (int i = 0; plist[i]; i++)
        if (strcmp(name, plist[i]) == 0) return 1;
    return 0;
}

int main(void)
{
    char    input[MSH_INPUT_SIZE];
    Command cmds[MAX_ARGS];
    int     ncmds;

    history_init(&g_history);

    printf(INFO_COLOR "Welcome to MiniShell — type 'help' for commands, 'exit' to quit.\n" RESET_COLOR);

    while (1) {
        print_prompt();

        if (!fgets(input, sizeof(input), stdin)) {
            printf("\n");
            break;
        }
        input[strcspn(input, "\n")] = '\0';

        char *trimmed = trim(input);
        if (!*trimmed)
            continue;

        history_add(&g_history, trimmed);

        ncmds = 0;
        if (parse_pipeline(trimmed, cmds, &ncmds) < 0) {
            fprintf(stderr, ERROR_COLOR "minishell: parse error\n" RESET_COLOR);
            continue;
        }
        if (ncmds == 0)
            continue;

        /* Run in parent only when: single command, no redirections, in plist */
        int in_parent = (ncmds == 1 &&
                         cmds[0].redir.input_file  == NULL &&
                         cmds[0].redir.output_file == NULL &&
                         must_run_in_parent(cmds[0].args[0]));

        if (in_parent) {
            int ret = run_builtin(&cmds[0], &g_history);
            if (ret == -42)
                break;
        } else {
            execute(cmds, ncmds);
        }

        free_commands(cmds, ncmds);
    }

    history_free(&g_history);
    printf(INFO_COLOR "Goodbye!\n" RESET_COLOR);
    return 0;
}
