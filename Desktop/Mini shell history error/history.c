#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "history.h"
 
/*
** Initialises all fields of the history struct to zero/NULL.
** Must be called once at shell startup before any other history function.
*/
void	history_init(t_history *h)
{
	int	i;
 
	i = 0;
	while (i < MAX_HISTORY)
	{
		h->entries[i] = NULL;
		i++;
	}
	h->count = 0;
	h->current_index = 0;
}
 
/*
** Duplicates a string using only malloc and standard string functions.
** Returns NULL if allocation fails.
** (Replaces ft_strdup if libft is not available yet.)
*/
static char	*dup_str(const char *s)
{
	char	*copy;
	size_t	len;
 
	if (!s)
		return (NULL);
	len = strlen(s);
	copy = malloc(len + 1);
	if (!copy)
		return (NULL);
	memcpy(copy, s, len + 1);
	return (copy);
}
 
/*
** Adds a command to the history.
** - Ignores empty strings and NULL.
** - Uses a circular buffer: when 100 entries are reached,
**   the oldest is overwritten.
** - After adding, resets current_index to end of history
**   so that the next UP arrow starts from the last command.
**
** Called by P1 in the main loop, after each validated input line.
** Example: history_add(&shell.history, shell.line);
*/
void	history_add(t_history *h, const char *cmd)
{
	int	idx;
 
	if (!cmd || !*cmd)
		return ;
	idx = h->count % MAX_HISTORY;
	if (h->entries[idx])
		free(h->entries[idx]);
	h->entries[idx] = dup_str(cmd);
	h->count++;
	h->current_index = h->count;
}
 
/*
** Prints the full history list to stdout, numbered from 1.
** Most recent entry is at the bottom.
** Output format matches bash's 'history' command.
**
** Example output:
**   1  ls -la
**   2  cd /tmp
**   3  echo hello
*/
void	history_print(t_history *h)
{
	int		total;
	int		start;
	int		i;
	int		display_num;
	int		idx;
 
	if (h->count == 0)
		return ;
	total = h->count;
	if (total > MAX_HISTORY)
	{
		start = total - MAX_HISTORY;
		total = MAX_HISTORY;
	}
	else
		start = 0;
	i = 0;
	while (i < total)
	{
		display_num = start + i + 1;
		idx = (start + i) % MAX_HISTORY;
		if (h->entries[idx])
			printf("%5d  %s\n", display_num, h->entries[idx]);
		i++;
	}
}
 
/*
** Returns the command at the given logical index (1-based, like bash).
** Returns NULL if the index is out of range.
**
** to implement the !n syntax (run command number n from history).
*/
char	*history_get(t_history *h, int index)
{
	int	start;
	int	real_index;
 
	if (index <= 0 || index > h->count)
		return (NULL);
	if (h->count > MAX_HISTORY)
		start = h->count - MAX_HISTORY;
	else
		start = 0;
	real_index = (start + index - 1) % MAX_HISTORY;
	return (h->entries[real_index]);
}
 
/*
** Moves the navigation cursor one step backward (UP arrow).
** Stops at the oldest available entry.
**
** After calling this, use history_current() to get the string.
*/
void	history_prev(t_history *h)
{
	int	min_index;
 
	if (h->count == 0)
		return ;
	if (h->count > MAX_HISTORY)
		min_index = h->count - MAX_HISTORY;
	else
		min_index = 0;
	if (h->current_index > min_index)
		h->current_index--;
}
 
/*
** Moves the navigation cursor one step forward (DOWN arrow).
** Stops at h->count (= one past the last entry = empty line).
**
*/
void	history_next(t_history *h)
{
	if (h->current_index < h->count)
		h->current_index++;
}
 
/*
** Returns the command string at the current navigation cursor.
** Returns NULL if the cursor is past the last entry
** (meaning the input line should be cleared, like in bash).
** to display the selected command in the prompt.
*/
char	*history_current(t_history *h)
{
	if (h->current_index >= h->count || h->current_index < 0)
		return (NULL);
	return (history_get(h, h->current_index + 1));
}
 
/*
** Frees all allocated strings in the history buffer.
** Must be called once at shell exit to avoid memory leaks.
** (Valgrind will flag any missing free here.)
**
** Example: history_free(&shell.history);
*/
void	history_free(t_history *h)
{
	int	i;
 
	i = 0;
	while (i < MAX_HISTORY)
	{
		if (h->entries[i])
		{
			free(h->entries[i]);
			h->entries[i] = NULL;
		}
		i++;
	}
	h->count = 0;
	h->current_index = 0;
}