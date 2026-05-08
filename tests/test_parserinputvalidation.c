#include "../include/parser.h"
#include <stdio.h>
#include <string.h>

void print_command(t_command *cmd)
{
    int i;
    
    if (!cmd)
    {
        printf("ERROR: NULL\n");
        return;
    }
    
    printf("✓ Command: %s | Args: %d | Pipe: %s | In: %s | Out: %s\n",
        cmd->name,
        cmd->argc,
        cmd->is_pipe_output ? "YES" : "NO",
        cmd->input_file ? cmd->input_file : "NONE",
        cmd->output_file ? cmd->output_file : "NONE");
}

void test_case(const char *description, const char *input, int should_succeed)
{
    t_command *cmd;
    
    printf("TEST: %s\n", description);
    printf("Input: \"%s\"\n", input);
    
    cmd = parse_command_line((char *)input);
    
    if (should_succeed)
    {
        if (cmd)
        {
            print_command(cmd);
            free_command(cmd);
            printf("✓ PASS\n\n");
        }
        else
        {
            printf("✗ FAIL - Expected success but got NULL\n\n");
        }
    }
    else
    {
        if (!cmd)
        {
            printf("✓ PASS (correctly rejected)\n\n");
        }
        else
        {
            printf("✗ FAIL - Expected failure but got success\n");
            free_command(cmd);
            printf("\n");
        }
    }
}

int main(void)
{
    printf("╔════════════════════════════════════════╗\n");
    printf("║   PARSER - EDGE CASE & STRESS TESTS   ║\n");
    printf("╚════════════════════════════════════════╝\n\n");
    
    // ===== BASIC VALID CASES =====
    printf("═══ BASIC VALID CASES ═══\n\n");
    test_case("Simple command", "ls", 1);
    test_case("Command with args", "ls -la /tmp", 1);
    
    // ===== QUOTED STRING EDGE CASES =====
    printf("═══ QUOTED STRING EDGE CASES ═══\n\n");
    test_case("Double quotes", "echo \"hello world\"", 1);
    test_case("Single quotes", "echo 'hello world'", 1);
    test_case("Empty double quotes", "echo \"\"", 1);
    test_case("Empty single quotes", "echo ''", 1);
    test_case("Single quotes in double", "echo \"it's\"", 1);
    test_case("Double quotes in single", "echo 'say \"hello\"'", 1);
    test_case("Pipe char in quotes", "echo \"cat | grep\"", 1);
    test_case("Redirect char in quotes", "echo \"save > file.txt\"", 1);
    test_case("Multiple spaces in quotes", "echo \"hello     world\"", 1);
    
    // ===== QUOTE VALIDATION ERRORS =====
    printf("═══ MISMATCHED QUOTES (SHOULD FAIL) ═══\n\n");
    test_case("Unclosed double quote", "echo \"hello", 0);
    test_case("Unclosed single quote", "echo 'hello", 0);
    test_case("Mixed unmatched quotes", "echo \"hello'", 0);
    
    // ===== REDIRECTION EDGE CASES =====
    printf("═══ REDIRECTION EDGE CASES ═══\n\n");
    test_case("Simple output >", "ls > file.txt", 1);
    test_case("Simple input <", "cat < file.txt", 1);
    test_case("Append >>", "echo data >> log.txt", 1);
    test_case("Both < and >", "cat < in.txt > out.txt", 1);
    test_case("Multiple spaces before filename", "ls >    file.txt", 1);
    test_case("Filename with path", "ls > /tmp/output.txt", 1);
    test_case("Filename with dots", "echo > file.txt.bak", 1);
    
    // ===== REDIRECTION ERRORS =====
    printf("═══ REDIRECTION ERRORS (SHOULD FAIL) ═══\n\n");
    test_case("Output without filename", "echo hello >", 0);
    test_case("Input without filename", "cat <", 0);
    test_case("Redirect to pipe", "ls > |", 0);
    test_case("Two redirects without file between", "ls > > file.txt", 0);
    
    // ===== PIPE EDGE CASES =====
    printf("═══ PIPE EDGE CASES ═══\n\n");
    test_case("Simple pipe", "ls | grep test", 1);
    test_case("Pipe with args", "ls -la | grep .c", 1);
    test_case("Pipe and redirection", "ls -la | grep .c > result.txt", 1);
    test_case("Pipe and input redirect", "cat < input.txt | grep test", 1);
    
    // ===== PIPE ERRORS =====
    printf("═══ PIPE ERRORS (SHOULD FAIL) ═══\n\n");
    test_case("Pipe with nothing before", "| grep test", 0);
    test_case("Pipe with nothing after", "ls |", 0);
    test_case("Multiple pipes", "ls | grep | wc", 1);  // Note: We only parse first command
    
    // ===== WHITESPACE EDGE CASES =====
    printf("═══ WHITESPACE EDGE CASES ═══\n\n");
    test_case("Leading spaces", "   ls -la", 1);
    test_case("Trailing spaces", "ls -la   ", 1);
    test_case("Extra spaces between args", "ls    -la    /tmp", 1);
    test_case("Tabs instead of spaces", "ls\t-la\t/tmp", 1);
    test_case("Mixed spaces and tabs", "ls \t -la  \t /tmp", 1);
    
    // ===== SPECIAL FILENAMES =====
    printf("═══ SPECIAL FILENAMES ═══\n\n");
    test_case("Filename with spaces (quoted)", "echo test > \"my file.txt\"", 1);
    test_case("Filename with dashes", "ls > my-output-file.txt", 1);
    test_case("Filename with numbers", "echo > file123.txt", 1);
    test_case("Filename with underscore", "ls > my_file.txt", 1);
    
    // ===== COMPLEX COMBINATIONS =====
    printf("═══ COMPLEX COMBINATIONS ═══\n\n");
    test_case("Everything together", "cat < \"input file.txt\" | grep \"pattern\" > \"output.txt\"", 1);
    test_case("Many args with quotes", "echo \"arg1\" arg2 \"arg 3\" arg4", 1);
    test_case("Quotes and redirects", "grep \"search term\" < data.txt > results.txt", 1);
    
    // ===== SPECIAL CASES =====
    printf("═══ SPECIAL CASES ═══\n\n");
    test_case("Just command name", "exit", 1);
    test_case("Command with dash flags", "ls -la -h -S", 1);
    test_case("Path as argument", "cd /usr/local/bin", 1);
    test_case("Relative path", "ls ./subdir", 1);
    test_case("Parent directory", "cd ..", 1);
    
    // ===== ERROR CASES =====
    printf("═══ ERROR CASES ═══\n\n");
    test_case("Empty input", "", 0);
    test_case("Only spaces", "   ", 0);
    test_case("Only tabs", "\t\t", 0);
    
    printf("╔════════════════════════════════════════╗\n");
    printf("║✅ EDGE CASE TESTING COMPLETE!        ║\n");
    printf("╚════════════════════════════════════════╝\n");
    
    return 0;
}
