# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: agalvan- <agalvan-@student.42madrid.com>   +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2026/10/05 00:00:00 by agalvan-          #+#    #+#              #
#    Updated: 2026/10/22 14:43:49 by agalvan-         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME = codexion

CC = cc

CFLAGS = -Wall -Wextra -Werror  -I./coders -O2 -MMD -MP -pthread

FT_MALLOC	?= $(HOME)/Documents/ft_malloc/libft_malloc.so
ARGS		?= 3 1000 200 200 200 1 200 fifo

SRC	=	  coders/core/codexion.c \
		  coders/core/coders.c \
		  coders/parse/parse.c \
		  coders/parse/parsing_utils.c \
		  coders/parse/display.c \
		  coders/utils/free.c \
		  coders/init/init_coders.c \
		  coders/init/init_dongles.c \
		  coders/init/init_threads.c \
		  coders/algo/heap.c \
		  coders/algo/heap_ops.c \

OBJS	= $(SRC:.c=.o)
DEPS	= $(SRC:.c=.d)

MAKEFLAGS += -s

all: $(NAME)

$(NAME): $(OBJS)
	@$(CC) $(CFLAGS) $(OBJS) -o $(NAME)
	@printf "\033[0;32m[BUILD OK] $(NAME) compiled successfully\033[0m\n"

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	@rm -rf $(OBJS) $(DEPS)

fclean: clean
	@rm -rf $(NAME)

re: fclean all

run: $(NAME)
	@test -f "$(FT_MALLOC)" \
		|| { printf "\033[0;31m[KO] falta $(FT_MALLOC)\033[0m\n"; \
			 printf "     compila ft_malloc con: make -C $$(dirname $(FT_MALLOC))\n"; \
			 exit 1; }
	@LD_PRELOAD=$(FT_MALLOC) ./$(NAME) $(ARGS)

native: $(NAME)
	@./$(NAME) $(ARGS)

.PHONY: all clean fclean re run native

-include $(DEPS)
