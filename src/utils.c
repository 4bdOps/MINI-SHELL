#include "minishell.h"

void print_error(const char *msg)
{
    fprintf(stderr, ERROR_COLOR "minishell: %s\n" RESET_COLOR, msg);
}

void print_prompt(void)
{
    char cwd[MAX_PATH];
    if (!getcwd(cwd, sizeof(cwd)))
        strncpy(cwd, "?", sizeof(cwd));

    /* Show only the last component to keep prompt short */
    char *last = strrchr(cwd, '/');
    const char *display = (last && *(last + 1)) ? last + 1 : cwd;

    printf(PROMPT_COLOR "minishell" RESET_COLOR
           ":" INFO_COLOR "%s" RESET_COLOR "$ ", display);
    fflush(stdout);
}
