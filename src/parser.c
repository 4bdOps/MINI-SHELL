#include "minishell.h"

/* ── helpers ──────────────────────────────────────────────── */

char *trim(char *s)
{
    while (*s == ' ' || *s == '\t') s++;
    char *end = s + strlen(s) - 1;
    while (end > s && (*end == ' ' || *end == '\t')) *end-- = '\0';
    return s;
}

/* Split `line` on `|`, return number of pipe-stages in `parts[]`.
   Each part is a NUL-terminated slice inside `buf`.               */
static int split_pipe(char *buf, char *parts[], int max)
{
    int n = 0;
    char *p = buf;
    char *start = buf;
    int in_q = 0;
    char qc = 0;

    while (*p) {
        if (!in_q && (*p == '\'' || *p == '"')) { in_q = 1; qc = *p; }
        else if (in_q && *p == qc)               { in_q = 0; }
        else if (!in_q && *p == '|') {
            if (n >= max - 1) return -1;
            *p = '\0';
            parts[n++] = start;
            start = p + 1;
        }
        p++;
    }
    parts[n++] = start;
    return n;
}

/* Tokenise a single stage (no pipes) into argv[] + redirections */
static int tokenise_stage(char *stage, Command *cmd)
{
    memset(cmd, 0, sizeof(*cmd));

    char *token;
    char *rest = stage;

    while ((token = strtok_r(rest, " \t", &rest))) {
        /* Input redirection */
        if (strcmp(token, "<") == 0) {
            char *file = strtok_r(NULL, " \t", &rest);
            if (!file) { fprintf(stderr, "minishell: expected filename after '<'\n"); return -1; }
            cmd->redir.input_file = strdup(file);
        }
        /* Append redirection */
        else if (strcmp(token, ">>") == 0) {
            char *file = strtok_r(NULL, " \t", &rest);
            if (!file) { fprintf(stderr, "minishell: expected filename after '>>'\n"); return -1; }
            cmd->redir.output_file = strdup(file);
            cmd->redir.append      = 1;
        }
        /* Output redirection */
        else if (strcmp(token, ">") == 0) {
            char *file = strtok_r(NULL, " \t", &rest);
            if (!file) { fprintf(stderr, "minishell: expected filename after '>'\n"); return -1; }
            cmd->redir.output_file = strdup(file);
            cmd->redir.append      = 0;
        }
        /* Inline redirection e.g. "file>" or ">file" */
        else if (token[0] == '>' && token[1] != '\0') {
            cmd->redir.output_file = strdup(token + 1);
        } else if (token[strlen(token)-1] == '>' ) {
            token[strlen(token)-1] = '\0';
            cmd->redir.output_file = strdup(token[0] ? token : "");
        }
        else {
            if (cmd->argc >= MAX_ARGS - 1) { fprintf(stderr, "minishell: too many arguments\n"); return -1; }
            cmd->args[cmd->argc++] = strdup(token);
        }
    }
    cmd->args[cmd->argc] = NULL;
    return 0;
}

/* ── public API ───────────────────────────────────────────── */

int parse_pipeline(char *line, Command cmds[], int *ncmds)
{
    /* work on a copy so we don't destroy the history string */
    char buf[MSH_INPUT_SIZE];
    strncpy(buf, line, MSH_INPUT_SIZE - 1);
    buf[MSH_INPUT_SIZE - 1] = '\0';

    char *parts[MAX_ARGS];
    int np = split_pipe(buf, parts, MAX_ARGS);
    if (np < 0) return -1;

    for (int i = 0; i < np; i++) {
        char *stage = trim(parts[i]);
        if (tokenise_stage(stage, &cmds[i]) < 0) return -1;
        if (cmds[i].argc == 0 && !cmds[i].redir.input_file && !cmds[i].redir.output_file) {
            /* empty stage – just skip */
            np--;
            i--;
            continue;
        }
    }
    *ncmds = np;
    return 0;
}

void free_commands(Command cmds[], int ncmds)
{
    for (int i = 0; i < ncmds; i++) {
        for (int j = 0; j < cmds[i].argc; j++) {
            free(cmds[i].args[j]);
            cmds[i].args[j] = NULL;
        }
        free(cmds[i].redir.input_file);
        free(cmds[i].redir.output_file);
    }
}
