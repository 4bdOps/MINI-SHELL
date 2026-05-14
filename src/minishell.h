#ifndef MINISHELL_H
#define MINISHELL_H

#define _POSIX_C_SOURCE 200809L
#define _DEFAULT_SOURCE          /* enables strdup, strtok_r, DT_DIR … */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <pwd.h>
#include <time.h>
#include <limits.h>

/* ─── Constants ─────────────────────────────────────────── */
#define MSH_INPUT_SIZE  4096
#define MAX_ARGS        256
#define MAX_HISTORY     100
#define MAX_PATH        1024
#define PROMPT_COLOR    "\033[1;32m"  /* bold green  */
#define RESET_COLOR     "\033[0m"
#define ERROR_COLOR     "\033[1;31m"  /* bold red    */
#define INFO_COLOR      "\033[1;36m"  /* bold cyan   */

/* ─── History ────────────────────────────────────────────── */
typedef struct {
    char  *entries[MAX_HISTORY];
    int    count;
    int    current;          /* index used during navigation */
} History;

/* ─── Redirection tokens ─────────────────────────────────── */
typedef struct {
    char  *input_file;       /* < file   */
    char  *output_file;      /* > file   */
    int    append;           /* 1 → >>   */
} Redirect;

/* ─── Pipeline stage ─────────────────────────────────────── */
typedef struct {
    char    *args[MAX_ARGS];
    int      argc;
    Redirect redir;
} Command;

/* ─── Prototypes ─────────────────────────────────────────── */

/* history.c */
void    history_init(History *h);
void    history_add(History *h, const char *line);
void    history_print(const History *h);
void    history_free(History *h);

/* parser.c */
int     parse_pipeline(char *line, Command cmds[], int *ncmds);
void    free_commands(Command cmds[], int ncmds);

/* builtins.c */
int     is_builtin(const char *cmd);
int     run_builtin(Command *cmd, History *h);

/* executor.c */
int     execute(Command cmds[], int ncmds);

/* utils.c */
void    print_error(const char *msg);
char   *trim(char *s);
void    print_prompt(void);

#endif /* MINISHELL_H */
