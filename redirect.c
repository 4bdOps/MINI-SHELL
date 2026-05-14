#include "../include/redirect.h"
#include "../include/error.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>

static int saved_stdin  = -1;
static int saved_stdout = -1;

int handle_redirection(t_command *cmd)
{
    int fd;

    if (!cmd)
        return 0;

    /* Save original fds */
    saved_stdin  = dup(STDIN_FILENO);
    saved_stdout = dup(STDOUT_FILENO);

    /* Input redirection: < file */
    if (cmd->input_file)
    {
        fd = open(cmd->input_file, O_RDONLY);
        if (fd < 0)
        {
            print_perror(cmd->input_file);
            return -1;
        }
        dup2(fd, STDIN_FILENO);
        close(fd);
    }

    /* Output redirection: > or >> file */
    if (cmd->output_file)
    {
        int flags = O_WRONLY | O_CREAT;
        flags |= cmd->append_output ? O_APPEND : O_TRUNC;

        fd = open(cmd->output_file, flags, 0644);
        if (fd < 0)
        {
            print_perror(cmd->output_file);
            return -1;
        }
        dup2(fd, STDOUT_FILENO);
        close(fd);
    }

    return 0;
}

void restore_redirection(void)
{
    if (saved_stdin >= 0)
    {
        dup2(saved_stdin, STDIN_FILENO);
        close(saved_stdin);
        saved_stdin = -1;
    }
    if (saved_stdout >= 0)
    {
        dup2(saved_stdout, STDOUT_FILENO);
        close(saved_stdout);
        saved_stdout = -1;
    }
}
