#   ██████╗  ██████╗ ██████╗ ███████╗██╗    ██╗██╗ ██████╗ ███╗   ██╗
#  ██╔════╝ ██╔═══██╗██╔══██╗██╔════╝ ██║  ██╔╝██║██╔═══██╗████╗  ██║
#  ██║      ██║   ██║██║  ██║█████╗    █████╔╝ ██║██║   ██║██╔██╗ ██║
#  ██║      ██║   ██║██║  ██║██╔══╝   ██╔═ ██╗ ██║██║   ██║██║╚██╗██║
#  ╚██████╗ ╚██████╔╝██████╔╝███████╗██║    ██╗██║╚██████╔╝██║ ╚████║
#   ╚═════╝  ╚═════╝ ╚═════╝ ╚══════╝╚═╝  ╚═╝╚═╝ ╚═════╝ ╚═╝  ╚═══╝

NAME		= codexion

CC			= cc
CFLAGS		= -Wall -Werror -Wextra -pthread -I coders/
DEPFLAGS	= -MMD -MP
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
DEPS		= $(SRCS:.c=.d)

# Colors
DEF_COLOR	= \033[0;39m
GRAY		= \033[0;90m
GREEN		= \033[0;92m

all: $(NAME)

$(NAME): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@echo "$(GREEN)Codexion compiled successfully!$(DEF_COLOR)"

%.o: %.c
	$(CC) $(CFLAGS) $(DEPFLAGS) -c $< -o $@

-include $(DEPS)

clean:
	$(RM) $(OBJS) $(DEPS)
	@echo "$(GRAY)Deleted objects.$(DEF_COLOR)"

fclean: clean
	$(RM) $(NAME)
	@echo "$(GRAY)Deleted executables.$(DEF_COLOR)"

re: fclean all

.PHONY: all clean fclean re