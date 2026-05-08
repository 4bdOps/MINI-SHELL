# ============================================================
# Mini Shell Makefile
# ============================================================

NAME		=	mini_shell

# Compiler and flags
CC			=	gcc
CFLAGS		=	-Wall -Wextra -Werror -I./include
LDFLAGS		=

# Source files
SRCS		=	src/main.c \
				src/parser.c \
				src/executor.c \
				src/builtin.c \
				src/history.c \
				src/error.c \
				src/redirect.c \
				src/scripts.c \
				src/utils.c

# Object files
OBJS		=	$(SRCS:.c=.o)

# ============================================================
# Rules
# ============================================================

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) -o $(NAME) $(OBJS) $(LDFLAGS)
	@echo " Build successful: ./$(NAME)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJS)
	@echo " Cleaned object files"

fclean: clean
	rm -f $(NAME)
	@echo " Removed executable"

re: fclean all

test: all
	@echo "Running mini_shell..."
	./$(NAME)

help:
	@echo "Available targets:"
	@echo "  make         - Compile the project"
	@echo "  make clean   - Remove object files"
	@echo "  make fclean  - Remove everything"
	@echo "  make re      - Rebuild from scratch"
	@echo "  make test    - Compile and run"
	@echo "  make help    - Show this help"

# Test parser
test_parser: $(OBJS)
	gcc -Wall -Wextra -I./include tests/test_parser.c src/parser.c -o test_parser
	./test_parser

# Run all tests
test: test_parser

.PHONY: test test_parser

.PHONY: all clean fclean re test help
