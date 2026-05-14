#include "minishell.h"

extern History g_history;

/* Apply redirections for a child process */
static int apply_redirections(Command *cmd)
{
    if (cmd->redir.input_file) {
        int fd = open(cmd->redir.input_file, O_RDONLY);
        if (fd < 0) { perror(cmd->redir.input_file); return -1; }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }
    if (cmd->redir.output_file) {
        int flags = O_WRONLY | O_CREAT | (cmd->redir.append ? O_APPEND : O_TRUNC);
        int fd = open(cmd->redir.output_file, flags, 0644);
        if (fd < 0) { perror(cmd->redir.output_file); return -1; }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }
    return 0;
}

/* Run a single command (may be builtin or external) in a child */
static void exec_child(Command *cmd)
{
    if (apply_redirections(cmd) < 0)
        exit(1);

    if (is_builtin(cmd->args[0])) {
        int ret = run_builtin(cmd, &g_history);
        exit(ret == -42 ? 0 : ret);
    }

    execvp(cmd->args[0], cmd->args);
    /* If we reach here execvp failed */
    fprintf(stderr, ERROR_COLOR "minishell: %s: command not found\n" RESET_COLOR,
            cmd->args[0]);
    exit(127);
}

/* ── main execute entry-point ─────────────────────────────── */
int execute(Command cmds[], int ncmds)
{
    if (ncmds <= 0) return 0;

    /* ── simple single command (no pipe) ── */
    if (ncmds == 1) {
        /* Built-ins that must run in the parent (cd, exit, etc.) are already
           handled in main.c; any that reach here run in a child so that
           redirections work correctly (e.g. "ls > out.txt").            */
        pid_t pid = fork();
        if (pid < 0) { perror("fork"); return 1; }
        if (pid == 0)
            exec_child(&cmds[0]);

        int status;
        waitpid(pid, &status, 0);
        return WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }

    /* ── pipeline ── */
    int   pipefds[2 * (ncmds - 1)];
    pid_t pids[ncmds];

    /* Create all pipes */
    for (int i = 0; i < ncmds - 1; i++) {
        if (pipe(pipefds + 2 * i) < 0) { perror("pipe"); return 1; }
    }

    for (int i = 0; i < ncmds; i++) {
        pids[i] = fork();
        if (pids[i] < 0) { perror("fork"); return 1; }

        if (pids[i] == 0) {
            /* Wire stdin from previous pipe */
            if (i > 0) {
                dup2(pipefds[2 * (i - 1)], STDIN_FILENO);
            }
            /* Wire stdout to next pipe */
            if (i < ncmds - 1) {
                dup2(pipefds[2 * i + 1], STDOUT_FILENO);
            }
            /* Close all pipe fds in child */
            for (int j = 0; j < 2 * (ncmds - 1); j++)
                close(pipefds[j]);

            exec_child(&cmds[i]);
        }
    }

    /* Parent closes all pipes */
    for (int i = 0; i < 2 * (ncmds - 1); i++)
        close(pipefds[i]);

    /* Wait for all children */
    int last_status = 0;
    for (int i = 0; i < ncmds; i++) {
        int status;
        waitpid(pids[i], &status, 0);
        if (i == ncmds - 1)
            last_status = WIFEXITED(status) ? WEXITSTATUS(status) : 1;
    }
    return last_status;
}
