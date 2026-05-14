#include "minishell.h"

extern History g_history;

/* ── built-in table ───────────────────────────────────────── */
static const char *BUILTINS[] = {
    "cd", "pwd", "ls", "mkdir", "rmdir", "rm", "touch",
    "cat", "echo", "env", "export", "unset",
    "history", "help", "clear", "exit", NULL
};

int is_builtin(const char *cmd)
{
    if (!cmd) return 0;
    for (int i = 0; BUILTINS[i]; i++)
        if (strcmp(cmd, BUILTINS[i]) == 0)
            return 1;
    return 0;
}

/* ── helpers ──────────────────────────────────────────────── */
static void print_err(const char *cmd, const char *msg)
{
    fprintf(stderr, ERROR_COLOR "minishell: %s: %s\n" RESET_COLOR, cmd, msg);
}

/* ── individual built-ins ─────────────────────────────────── */

/* cd [dir] */
static int builtin_cd(Command *cmd)
{
    const char *dir;
    if (cmd->argc < 2) {
        dir = getenv("HOME");
        if (!dir) { print_err("cd", "HOME not set"); return 1; }
    } else {
        dir = cmd->args[1];
    }
    if (chdir(dir) < 0) {
        perror("cd");
        return 1;
    }
    /* update PWD */
    char cwd[MAX_PATH];
    if (getcwd(cwd, sizeof(cwd)))
        setenv("PWD", cwd, 1);
    return 0;
}

/* pwd */
static int builtin_pwd(void)
{
    char cwd[MAX_PATH];
    if (!getcwd(cwd, sizeof(cwd))) { perror("pwd"); return 1; }
    printf("%s\n", cwd);
    return 0;
}

/* ls [-l] [-a] [path] */
static int builtin_ls(Command *cmd)
{
    const char *path = ".";
    int long_fmt = 0, show_all = 0;

    for (int i = 1; i < cmd->argc; i++) {
        if (strcmp(cmd->args[i], "-l") == 0)       long_fmt  = 1;
        else if (strcmp(cmd->args[i], "-a") == 0)   show_all  = 1;
        else if (strcmp(cmd->args[i], "-la") == 0 ||
                 strcmp(cmd->args[i], "-al") == 0) { long_fmt = show_all = 1; }
        else                                         path = cmd->args[i];
    }

    DIR *dp = opendir(path);
    if (!dp) { perror("ls"); return 1; }

    struct dirent *de;
    while ((de = readdir(dp))) {
        if (!show_all && de->d_name[0] == '.')
            continue;

        if (long_fmt) {
            char fullpath[MAX_PATH];
            snprintf(fullpath, sizeof(fullpath), "%s/%s", path, de->d_name);
            struct stat st;
            if (stat(fullpath, &st) < 0) { perror(de->d_name); continue; }

            /* permissions */
            char perms[11];
            perms[0] = S_ISDIR(st.st_mode)  ? 'd' :
                       S_ISLNK(st.st_mode)  ? 'l' : '-';
            perms[1] = (st.st_mode & S_IRUSR) ? 'r' : '-';
            perms[2] = (st.st_mode & S_IWUSR) ? 'w' : '-';
            perms[3] = (st.st_mode & S_IXUSR) ? 'x' : '-';
            perms[4] = (st.st_mode & S_IRGRP) ? 'r' : '-';
            perms[5] = (st.st_mode & S_IWGRP) ? 'w' : '-';
            perms[6] = (st.st_mode & S_IXGRP) ? 'x' : '-';
            perms[7] = (st.st_mode & S_IROTH) ? 'r' : '-';
            perms[8] = (st.st_mode & S_IWOTH) ? 'w' : '-';
            perms[9] = (st.st_mode & S_IXOTH) ? 'x' : '-';
            perms[10]= '\0';

            char timebuf[32];
            strftime(timebuf, sizeof(timebuf), "%b %d %H:%M",
                     localtime(&st.st_mtime));

            if (S_ISDIR(st.st_mode))
                printf("%s %4ld %8lld %s " INFO_COLOR "%s/" RESET_COLOR "\n",
                       perms, (long)st.st_nlink, (long long)st.st_size,
                       timebuf, de->d_name);
            else
                printf("%s %4ld %8lld %s %s\n",
                       perms, (long)st.st_nlink, (long long)st.st_size,
                       timebuf, de->d_name);
        } else {
            if (de->d_type == DT_DIR)
                printf(INFO_COLOR "%s/" RESET_COLOR "  ", de->d_name);
            else
                printf("%s  ", de->d_name);
        }
    }
    if (!long_fmt) printf("\n");
    closedir(dp);
    return 0;
}

/* mkdir <dir> [...] */
static int builtin_mkdir(Command *cmd)
{
    if (cmd->argc < 2) { print_err("mkdir", "missing operand"); return 1; }
    int rc = 0;
    for (int i = 1; i < cmd->argc; i++) {
        if (mkdir(cmd->args[i], 0755) < 0) { perror(cmd->args[i]); rc = 1; }
    }
    return rc;
}

/* rmdir <dir> [...] */
static int builtin_rmdir(Command *cmd)
{
    if (cmd->argc < 2) { print_err("rmdir", "missing operand"); return 1; }
    int rc = 0;
    for (int i = 1; i < cmd->argc; i++) {
        if (rmdir(cmd->args[i]) < 0) { perror(cmd->args[i]); rc = 1; }
    }
    return rc;
}

/* rm [-r] <file> [...] */
static int builtin_rm(Command *cmd)
{
    if (cmd->argc < 2) { print_err("rm", "missing operand"); return 1; }
    /* delegate to system rm for recursive support */
    return execute(cmd, 1);   /* let executor handle it as external */
    (void)cmd;
    return 0;
}

/* touch <file> [...] */
static int builtin_touch(Command *cmd)
{
    if (cmd->argc < 2) { print_err("touch", "missing operand"); return 1; }
    int rc = 0;
    for (int i = 1; i < cmd->argc; i++) {
        int fd = open(cmd->args[i], O_CREAT | O_WRONLY, 0644);
        if (fd < 0) { perror(cmd->args[i]); rc = 1; }
        else close(fd);
    }
    return rc;
}

/* cat <file> [...] */
static int builtin_cat(Command *cmd)
{
    if (cmd->argc < 2) {
        /* read stdin */
        char buf[1024];
        while (fgets(buf, sizeof(buf), stdin))
            fputs(buf, stdout);
        return 0;
    }
    int rc = 0;
    for (int i = 1; i < cmd->argc; i++) {
        FILE *f = fopen(cmd->args[i], "r");
        if (!f) { perror(cmd->args[i]); rc = 1; continue; }
        char buf[1024];
        while (fgets(buf, sizeof(buf), f))
            fputs(buf, stdout);
        fclose(f);
    }
    return rc;
}

/* echo [-n] <args...> */
static int builtin_echo(Command *cmd)
{
    int no_newline = 0;
    int start = 1;
    if (cmd->argc > 1 && strcmp(cmd->args[1], "-n") == 0) {
        no_newline = 1;
        start = 2;
    }
    for (int i = start; i < cmd->argc; i++) {
        if (i > start) putchar(' ');
        fputs(cmd->args[i], stdout);
    }
    if (!no_newline) putchar('\n');
    return 0;
}

/* env */
static int builtin_env(void)
{
    extern char **environ;
    for (char **e = environ; *e; e++)
        printf("%s\n", *e);
    return 0;
}

/* export VAR=VALUE */
static int builtin_export(Command *cmd)
{
    if (cmd->argc < 2) { builtin_env(); return 0; }
    for (int i = 1; i < cmd->argc; i++) {
        char *eq = strchr(cmd->args[i], '=');
        if (!eq) {
            /* just mark existing var for export — nothing to do in this shell */
            continue;
        }
        *eq = '\0';
        setenv(cmd->args[i], eq + 1, 1);
        *eq = '=';
    }
    return 0;
}

/* unset VAR */
static int builtin_unset(Command *cmd)
{
    for (int i = 1; i < cmd->argc; i++)
        unsetenv(cmd->args[i]);
    return 0;
}

/* history */
static int builtin_history(History *h)
{
    history_print(h);
    return 0;
}

/* help */
static int builtin_help(void)
{
    printf(INFO_COLOR
           "╔══════════════════════════════════════╗\n"
           "║          MiniShell — Help            ║\n"
           "╚══════════════════════════════════════╝\n"
           RESET_COLOR);
    printf("  cd [dir]          Change directory\n");
    printf("  pwd               Print working directory\n");
    printf("  ls [-l] [-a] [d]  List directory contents\n");
    printf("  mkdir <dir>       Create directory\n");
    printf("  rmdir <dir>       Remove empty directory\n");
    printf("  rm <file>         Remove file\n");
    printf("  touch <file>      Create/update file\n");
    printf("  cat <file>        Display file\n");
    printf("  echo [args]       Print text\n");
    printf("  env               Show environment\n");
    printf("  export VAR=VAL    Set environment variable\n");
    printf("  unset VAR         Unset variable\n");
    printf("  history           Show command history\n");
    printf("  clear             Clear screen\n");
    printf("  exit [code]       Exit shell\n");
    printf("\n  Pipes:       cmd1 | cmd2\n");
    printf("  Redirect:    cmd > file   cmd >> file   cmd < file\n");
    printf("  External:    Any command found in PATH is also supported.\n");
    return 0;
}

/* clear */
static int builtin_clear(void)
{
    printf("\033[2J\033[H");
    fflush(stdout);
    return 0;
}

/* exit [code] */
static int builtin_exit(Command *cmd)
{
    int code = 0;
    if (cmd->argc > 1) code = atoi(cmd->args[1]);
    printf(INFO_COLOR "exit %d\n" RESET_COLOR, code);
    /* Return special sentinel so main() knows to quit */
    return -42;
}

/* ── dispatcher ───────────────────────────────────────────── */
int run_builtin(Command *cmd, History *h)
{
    if (!cmd || cmd->argc == 0) return 0;
    const char *name = cmd->args[0];

    if (strcmp(name, "cd")      == 0) return builtin_cd(cmd);
    if (strcmp(name, "pwd")     == 0) return builtin_pwd();
    if (strcmp(name, "ls")      == 0) return builtin_ls(cmd);
    if (strcmp(name, "mkdir")   == 0) return builtin_mkdir(cmd);
    if (strcmp(name, "rmdir")   == 0) return builtin_rmdir(cmd);
    if (strcmp(name, "rm")      == 0) return builtin_rm(cmd);
    if (strcmp(name, "touch")   == 0) return builtin_touch(cmd);
    if (strcmp(name, "cat")     == 0) return builtin_cat(cmd);
    if (strcmp(name, "echo")    == 0) return builtin_echo(cmd);
    if (strcmp(name, "env")     == 0) return builtin_env();
    if (strcmp(name, "export")  == 0) return builtin_export(cmd);
    if (strcmp(name, "unset")   == 0) return builtin_unset(cmd);
    if (strcmp(name, "history") == 0) return builtin_history(h);
    if (strcmp(name, "help")    == 0) return builtin_help();
    if (strcmp(name, "clear")   == 0) return builtin_clear();
    if (strcmp(name, "exit")    == 0) return builtin_exit(cmd);

    print_err(name, "unknown built-in");
    return 1;
}
