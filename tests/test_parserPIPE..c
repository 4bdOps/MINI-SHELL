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
        printf("⚡ Reads from pipe\n");
    if (cmd->is_pipe_output)
        printf("⚡ Writes to pipe\n");
    
    printf("\n");
}

int main(void)
{
    t_command *cmd;
    
    printf("╔════════════════════════════════════════╗\n");
    printf("║    MINI SHELL PARSER - FULL TEST SUITE║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    // ===== BASIC TESTS =====
    printf("═══ BASIC COMMANDS ═══\n\n");
    
    printf("TEST 1: Simple command\n");
    cmd = parse_command_line("ls");
    print_command(cmd);
    free_command(cmd);
    
    printf("TEST 2: Command with args\n");
    cmd = parse_command_line("ls -la /tmp");
    print_command(cmd);
    free_command(cmd);
    
    // ===== REDIRECTION TESTS =====
    printf("═══ REDIRECTION ═══\n\n");
    
    printf("TEST 3: Output redirection >\n");
    cmd = parse_command_line("echo hello > output.txt");
    print_command(cmd);
    free_command(cmd);
    
    printf("TEST 4: Input redirection <\n");
    cmd = parse_command_line("cat < input.txt");
    print_command(cmd);
    free_command(cmd);
    
    printf("TEST 5: Both < and >\n");
    cmd = parse_command_line("cat < in.txt > out.txt");
    print_command(cmd);
    free_command(cmd);
    
    printf("TEST 6: Append redirection >>\n");
    cmd = parse_command_line("echo data >> log.txt");
    print_command(cmd);
    free_command(cmd);
    
    // ===== PIPE TESTS =====
    printf("═══ PIPES ═══\n\n");
    
    printf("TEST 7: Simple pipe\n");
    cmd = parse_command_line("ls | grep .c");
    print_command(cmd);
    free_command(cmd);
    
    printf("TEST 8: Pipe with args\n");
    cmd = parse_command_line("ls -la | grep test");
    print_command(cmd);
    free_command(cmd);
    
    printf("TEST 9: Pipe with redirection\n");
    cmd = parse_command_line("ls -la | grep .c > result.txt");
    print_command(cmd);
    free_command(cmd);
    
    // ===== QUOTED STRING TESTS =====
    printf("═══ QUOTED STRINGS ═══\n\n");
    
    printf("TEST 10: Double quotes\n");
    cmd = parse_command_line("echo \"hello world\"");
    print_command(cmd);
    free_command(cmd);
    
    printf("TEST 11: Single quotes\n");
    cmd = parse_command_line("echo 'hello world'");
    print_command(cmd);
    free_command(cmd);
    
    printf("TEST 12: Multiple quoted args\n");
    cmd = parse_command_line("echo \"hello\" \"world\" \"test\"");
    print_command(cmd);
    free_command(cmd);
    
    printf("TEST 13: Quoted with pipe\n");
    cmd = parse_command_line("echo \"searching for test\" | grep test");
    print_command(cmd);
    free_command(cmd);
    
    printf("TEST 14: Quoted with redirection\n");
    cmd = parse_command_line("echo \"hello world\" > file.txt");
    print_command(cmd);
    free_command(cmd);
    
    // ===== COMPLEX TESTS =====
    printf("═══ COMPLEX COMBINATIONS ═══\n\n");
    
    printf("TEST 15: Everything together\n");
    cmd = parse_command_line("cat < input.txt | grep \"pattern\" > output.txt");
    print_command(cmd);
    free_command(cmd);
    
    printf("TEST 16: Multiple spaces\n");
    cmd = parse_command_line("ls    -la    /tmp");
    print_command(cmd);
    free_command(cmd);
    
    printf("TEST 17: Leading spaces\n");
    cmd = parse_command_line("   echo \"test\"");
    print_command(cmd);
    free_command(cmd);
    
    printf("TEST 18: Quoted string with spaces\n");
    cmd = parse_command_line("printf \"this is a long string with spaces\"");
    print_command(cmd);
    free_command(cmd);
    
    // ===== ERROR CASES =====
    printf("═══ ERROR HANDLING ═══\n\n");
    
    printf("TEST 19: Empty input (should fail)\n");
    cmd = parse_command_line("");
    if (!cmd)
        printf("✓ Correctly returned NULL\n");
    printf("\n");
    
    printf("TEST 20: Just spaces (should fail)\n");
    cmd = parse_command_line("   ");
    if (!cmd)
        printf("✓ Correctly returned NULL\n");
    printf("\n");
    
    printf("╔════════════════════════════════════════╗\n");
    printf("║✅ ALL PARSER TESTS COMPLETED!        ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    return 0;
}
