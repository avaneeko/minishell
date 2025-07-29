# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: losypenk <losypenk@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/24 17:33:26 by losypenk          #+#    #+#              #
#    Updated: 2025/07/25 19:34:22 by losypenk         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := minishell

SRC := src/main.c src/utils.c src/utils2.c
OBJ := $(SRC:src/%.c=obj/%.o)

CC := cc

CFLAGS := -Wall -Wextra -Werror -Isrc -MMD -MP $(CF)
LDFLAGS := -lreadline $(LDF)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

obj:
	mkdir obj

obj/%.o: src/%.c obj
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean
	+$(MAKE) all

-include $(OBJ:.o=.d)
