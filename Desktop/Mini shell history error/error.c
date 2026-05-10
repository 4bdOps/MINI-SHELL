#include <stdio.h>
#include "errors.h"
 
int	g_last_exit_code = 0;
 
/*
** Returns the standard POSIX exit code for each error type.
*/
int	get_exit_code(t_error code)
{
	if (code == ERR_CMD_NOT_FOUND)
		return (127);
	if (code == ERR_PERMISSION)
		return (126);
	if (code == ERR_SYNTAX)
		return (2);
	if (code == ERR_MALLOC)
		return (1);
	if (code == ERR_NO_SUCH_FILE)
		return (1);
	if (code == ERR_PIPE_FAILED)
		return (1);
	if (code == ERR_FORK_FAILED)
		return (1);
	if (code == ERR_TOO_MANY_ARGS)
		return (1);
	if (code == ERR_NUMERIC_ARG)
		return (255);
	return (0);
}
 
/*
** Prints a formatted error message to stderr.
** context can be NULL for generic messages (e.g. syntax errors).
*/
void	print_error(t_error code, const char *context)
{
	if (code == ERR_NONE)
		return ;
	if (code == ERR_CMD_NOT_FOUND)
	{
		if (context)
			fprintf(stderr, "minishell: %s: command not found\n", context);
		else
			fprintf(stderr, "minishell: command not found\n");
	}
	else if (code == ERR_PERMISSION)
	{
		if (context)
			fprintf(stderr, "minishell: %s: Permission denied\n", context);
		else
			fprintf(stderr, "minishell: Permission denied\n");
	}
	else if (code == ERR_NO_SUCH_FILE)
	{
		if (context)
			fprintf(stderr, "minishell: %s: No such file or directory\n",
				context);
		else
			fprintf(stderr, "minishell: No such file or directory\n");
	}
	else if (code == ERR_SYNTAX)
	{
		if (context)
			fprintf(stderr, "minishell: syntax error near unexpected token"
				" `%s'\n", context);
		else
			fprintf(stderr, "minishell: syntax error\n");
	}
	else if (code == ERR_PIPE_FAILED)
		fprintf(stderr, "minishell: pipe: failed to create pipe\n");
	else if (code == ERR_FORK_FAILED)
		fprintf(stderr, "minishell: fork: failed to create process\n");
	else if (code == ERR_MALLOC)
		fprintf(stderr, "minishell: fatal: memory allocation failed\n");
	else if (code == ERR_TOO_MANY_ARGS)
	{
		if (context)
			fprintf(stderr, "minishell: %s: too many arguments\n", context);
	}
	else if (code == ERR_NUMERIC_ARG)
	{
		if (context)
			fprintf(stderr,
				"minishell: exit: %s: numeric argument required\n", context);
	}
	set_last_exit_code(get_exit_code(code));
}
 
/*
** Stores the exit code of the last executed command.
** This is what $? reads.
*/
void	set_last_exit_code(int code)
{
	g_last_exit_code = code;
}
 
/*
** Returns the exit code of the last executed command.
*/
int	get_last_exit_code(void)
{
	return (g_last_exit_code);
}