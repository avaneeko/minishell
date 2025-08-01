# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/24 17:33:26 by losypenk          #+#    #+#              #
#    Updated: 2025/08/01 16:44:51 by losypenk         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := minishell

SRC := src/main.c src/utils.c src/utils2.c src/token.c src/token_list.c \
	src/token_list2.c src/tokenizer.c
OBJ := $(SRC:src/%.c=obj/%.o)

CC := cc

CFLAGS := -Wall -Wextra -Werror -Isrc -MMD -MP $(CF)
LDFLAGS := -lreadline $(LDF)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

obj:
	mkdir -p obj

obj/%.o: src/%.c obj
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean
	+$(MAKE) all

-include $(OBJ:.o=.d)
