#ifndef ERRORS_H
# define ERRORS_H
 
typedef enum e_error
{
	ERR_NONE = 0,
	ERR_CMD_NOT_FOUND,
	ERR_PERMISSION,
	ERR_NO_SUCH_FILE,
	ERR_SYNTAX,
	ERR_PIPE_FAILED,
	ERR_FORK_FAILED,
	ERR_MALLOC,
	ERR_TOO_MANY_ARGS,
	ERR_NUMERIC_ARG
}	t_error;
 
extern int	g_last_exit_code;
 
void		print_error(t_error code, const char *context);
int			get_exit_code(t_error code);
void		set_last_exit_code(int code);
int			get_last_exit_code(void);
 
#endif