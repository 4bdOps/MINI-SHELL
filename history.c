#include "../include/history.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HISTORY 100

static char *history[MAX_HISTORY];
static int   history_count = 0;

void add_to_history(char *command)
{
    if (command == NULL || command[0] == '\0')
        return;

    /* Éviter les doublons consécutifs */
    if (history_count > 0 &&
        strcmp(history[history_count - 1], command) == 0)
        return;

    if (history_count < MAX_HISTORY)
    {
        history[history_count] = strdup(command);
        history_count++;
    }
    else
    {
        /* Buffer circulaire : libérer la plus ancienne, décaler */
        free(history[0]);
        for (int i = 0; i < MAX_HISTORY - 1; i++)
            history[i] = history[i + 1];
        history[MAX_HISTORY - 1] = strdup(command);
    }
}

void print_history(void)
{
    for (int i = 0; i < history_count; i++)
        printf("  %3d  %s\n", i + 1, history[i]);
}

void save_history(void)
{
    FILE *f = fopen(".mini_shell_history", "w");
    if (f == NULL)
        return;
    for (int i = 0; i < history_count; i++)
        fprintf(f, "%s\n", history[i]);
    fclose(f);
}

void load_history(void)
{
    char  line[1024];
    FILE *f = fopen(".mini_shell_history", "r");
    if (f == NULL)
        return;
    while (fgets(line, sizeof(line), f) != NULL)
    {
        line[strcspn(line, "\n")] = '\0';
        if (line[0] != '\0')
            add_to_history(line);
    }
    fclose(f);
}
