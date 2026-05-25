#   ██████╗  ██████╗ ██████╗ ███████╗██╗    ██╗██╗ ██████╗ ███╗   ██╗
#  ██╔════╝ ██╔═══██╗██╔══██╗██╔════╝ ██║  ██╔╝██║██╔═══██╗████╗  ██║
#  ██║      ██║   ██║██║  ██║█████╗    █████╔╝ ██║██║   ██║██╔██╗ ██║
#  ██║      ██║   ██║██║  ██║██╔══╝   ██╔═ ██╗ ██║██║   ██║██║╚██╗██║
#  ╚██████╗ ╚██████╔╝██████╔╝███████╗██║    ██╗██║╚██████╔╝██║ ╚████║
#   ╚═════╝  ╚═════╝ ╚═════╝ ╚══════╝╚═╝  ╚═╝╚═╝ ╚═════╝ ╚═╝  ╚═══╝

NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Werror -Wextra -pthread -I coders/
RM			= rm -rf

SRCS		= coders/main.c \
			coders/parsing.c \
			coders/init.c \
			coders/utils.c \
			coders/actions.c \
			coders/actions_utils.c \
			coders/routine.c \
			coders/monitor.c \
			coders/heap.c \
			coders/heap_utils.c

OBJS		= $(SRCS:.c=.o)

# Colors
DEF_COLOR   = \033[0;39m
GRAY        = \033[0;90m
GREEN       = \033[0;92m

all: $(NAME)

# target : dependencies
#	command
$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)Codexion compiled successfuly!$(DEF_COLOR)"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJS)
	@echo "$(GRAY)Deleted objects.$(DEF_COLOR)"

fclean: clean
	$(RM) $(NAME) 
	@echo "$(GRAY)Deleted Executables.$(DEF_COLOR)"

re: fclean all

.PHONY: all clean fclean re

