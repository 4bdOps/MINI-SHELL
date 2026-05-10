NAME		= test_p3
 
CC			= gcc
CFLAGS		= -Wall -Wextra -Werror
 
SRCS		= history.c errors.c test_p3.c
OBJS		= $(SRCS:.c=.o)
 
all: $(NAME)
 
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
 
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@
 
clean:
	rm -f $(OBJS)
 
fclean: clean
	rm -f $(NAME)
 
re: fclean all
 
valgrind: all
	valgrind --leak-check=full --track-origins=yes ./$(NAME)
 
.PHONY: all clean fclean re valgrind