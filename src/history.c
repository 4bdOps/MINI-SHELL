#include "minishell.h"

void history_init(History *h)
{
    memset(h, 0, sizeof(*h));
}

void history_add(History *h, const char *line)
{
    if (!line || !*line)
        return;

    /* Avoid duplicate of the very last entry */
    if (h->count > 0 && strcmp(h->entries[h->count - 1], line) == 0)
        return;

    if (h->count < MAX_HISTORY) {
        h->entries[h->count++] = strdup(line);
    } else {
        /* Circular overwrite: shift everything up, drop oldest */
        free(h->entries[0]);
        memmove(h->entries, h->entries + 1,
                (MAX_HISTORY - 1) * sizeof(char *));
        h->entries[MAX_HISTORY - 1] = strdup(line);
    }
    h->current = h->count;
}

void history_print(const History *h)
{
    if (h->count == 0) {
        printf("  (history is empty)\n");
        return;
    }
    for (int i = 0; i < h->count; i++)
        printf(INFO_COLOR "  %3d" RESET_COLOR "  %s\n", i + 1, h->entries[i]);
}

void history_free(History *h)
{
    for (int i = 0; i < h->count; i++) {
        free(h->entries[i]);
        h->entries[i] = NULL;
    }
    h->count   = 0;
    h->current = 0;
}
