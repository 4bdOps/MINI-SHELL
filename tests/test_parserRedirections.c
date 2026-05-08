#include "../include/parser.h"
#include <stdio.h>
#include <string.h>

void print_command(t_command *cmd)
{
    int i;
    
    if (!cmd)
    {
        printf("ERROR: Command is NULL\n");
        return;
    }
    
    printf("Command: %s\n", cmd->name);
    printf("Argc: %d\n", cmd->argc);
    printf("Args:\n");
    
    i = 0;
    while (i < cmd->argc)
    {
        printf("  [%d] = \"%s\"\n", i, cmd->args[i]);
        i++;
    }
    
    if (cmd->input_file)
        printf("Input file: %s\n", cmd->input_file);
    if (cmd->output_file)
        printf("Output file: %s (append: %d)\n", cmd->output_file, cmd->append_output);
    if (cmd->is_pipe_input)
        printf("Reads from pipe\n");
    if (cmd->is_pipe_output)
        printf("Writes to pipe\n");
    
    printf("\n");
}

int main(void)
{
    t_command *cmd;
    
    printf("==== TEST 1: Simple command ====\n");
    cmd = parse_command_line("ls");
    print_command(cmd);
    free_command(cmd);
    
    printf("==== TEST 2: Command with args ====\n");
    cmd = parse_command_line("ls -la /tmp");
    print_command(cmd);
    free_command(cmd);
    
    printf("==== TEST 3: Output redirection > ====\n");
    cmd = parse_command_line("echo hello > output.txt");
    print_command(cmd);
    free_command(cmd);
    
    printf("==== TEST 4: Input redirection < ====\n");
    cmd = parse_command_line("cat < input.txt");
    print_command(cmd);
    free_command(cmd);
    
    printf("==== TEST 5: Both input and output ====\n");
    cmd = parse_command_line("cat < input.txt > output.txt");
    print_command(cmd);
    free_command(cmd);
    
    printf("==== TEST 6: Append redirection >> ====\n");
    cmd = parse_command_line("echo data >> logfile.txt");
    print_command(cmd);
    free_command(cmd);
    
    printf("==== TEST 7: Command with args and redirection ====\n");
    cmd = parse_command_line("ls -la /tmp > list.txt");
    print_command(cmd);
    free_command(cmd);
    
    printf("\n✅ All parser tests completed!\n");
    
    return 0;
}
