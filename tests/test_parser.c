#include "../include/parser.h"
#include <stdio.h>
#include <string.h>

/**
 * Simple test program to verify parser works
 */

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
    printf("\n");
}

int main(void)
{
    t_command *cmd;
    
    printf("==== TEST 1: Simple command ====\n");
    cmd = parse_command_line("ls");
    if (cmd)
    {
        print_command(cmd);
        free_command(cmd);
    }
    
    printf("==== TEST 2: Command with one argument ====\n");
    cmd = parse_command_line("cd /tmp");
    if (cmd)
    {
        print_command(cmd);
        free_command(cmd);
    }
    
    printf("==== TEST 3: Command with multiple arguments ====\n");
    cmd = parse_command_line("ls -la /tmp");
    if (cmd)
    {
        print_command(cmd);
        free_command(cmd);
    }
    
    printf("==== TEST 4: Command with many args ====\n");
    cmd = parse_command_line("echo hello world this is a test");
    if (cmd)
    {
        print_command(cmd);
        free_command(cmd);
    }
    
    printf("==== TEST 5: Extra spaces ====\n");
    cmd = parse_command_line("ls   -la   /tmp");
    if (cmd)
    {
        print_command(cmd);
        free_command(cmd);
    }
    
    printf("==== TEST 6: Leading spaces ====\n");
    cmd = parse_command_line("   ls -la");
    if (cmd)
    {
        print_command(cmd);
        free_command(cmd);
    }
    
    printf("==== TEST 7: Empty input (should fail) ====\n");
    cmd = parse_command_line("");
    if (!cmd)
        printf("✓ Correctly returned NULL\n");
    
    printf("\n✅ All parser tests completed!\n");
    
    return 0;
}
