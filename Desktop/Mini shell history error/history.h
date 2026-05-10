#ifndef HISTORY_H
# define HISTORY_H
 
# define MAX_HISTORY 100
 
typedef struct s_history
{
	char	*entries[MAX_HISTORY];
	int		count;
	int		current_index;
}	t_history;
 
void	history_init(t_history *h);
void	history_add(t_history *h, const char *cmd);
void	history_print(t_history *h);
char	*history_get(t_history *h, int index);
void	history_prev(t_history *h);
void	history_next(t_history *h);
char	*history_current(t_history *h);
void	history_free(t_history *h);
 
#endif