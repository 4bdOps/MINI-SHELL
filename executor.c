#include "../include/executor.h"
#include "../include/builtin.h"
#include "../include/redirect.h"
#include "../include/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * execute_command - Execute a parsed command (builtin or external)
 */
int execute_command(t_command *cmd)
{
    pid_t pid;
    int   status;

    if (!cmd || !cmd->name)
    {
        fprintf(stderr, "Error: Invalid command\n");
        return -1;
    }

    /* Builtins */
    if (is_builtin(cmd->name))
        return execute_builtin(cmd);

    /* External command via fork/execvp */
    pid = fork();
    if (pid < 0)
    {
        print_perror("fork");
        return -1;
    }

    if (pid == 0)
    {
        /* Child: handle redirections then exec */
        if (handle_redirection(cmd) < 0)
            exit(EXIT_FAILURE);

        execvp(cmd->args[0], cmd->args);
        fprintf(stderr, "mini_shell: %s: command not found\n", cmd->args[0]);
        exit(EXIT_FAILURE);
    }

    /* Parent: restore redirections and wait */
    restore_redirection();
    waitpid(pid, &status, 0);
    return WIFEXITED(status) ? WEXITSTATUS(status) : -1;
}
