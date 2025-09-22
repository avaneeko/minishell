# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: myli-pen <myli-pen@student.hive.fi>        +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/24 17:33:26 by losypenk          #+#    #+#              #
#    Updated: 2025/09/22 15:53:57 by myli-pen         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := minishell

SRC := src/main.c src/utils.c src/utils2.c src/utils3.c src/token.c \
	src/token_list.c src/token_list2.c src/tokenizer.c src/app_create.c \
	src/app_destroy.c src/env.c src/expansion.c src/astr.c src/token_resplit.c \
	src/heredoc.c src/expansion_utils.c src/heredoc_bad_good_heredoc.c \
	src/heredoc_expand_prompt.c src/heredoc_get_file_name.c src/heredoc_input.c\
	src/heredoc_open_write.c
	src/Builtins/builtin_cd.c \
	src/Builtins/builtin_echo.c \
	src/Builtins/builtin_env.c \
	src/Builtins/builtin_exit.c \
	src/Builtins/builtin_export.c \
	src/Builtins/builtin_pwd.c \
	src/Builtins/builtin_unset.c \
	src/Builtins/builtins_utils.c \
	src/Builtins/env_helpers.c \
	src/Execution/command_path.c \
	src/Execution/execution_env.c \
	src/Execution/execution_exec.c \
	src/Execution/execution_resources.c \
	src/Execution/execution_utils.c \
	src/Execution/execution_utils2.c \
	src/Execution/execution_utils3.c \
	src/Execution/executor.c \
	src/Execution/redirection.c \
	src/Signals/exit_status.c \
	src/Signals/signals.c \

OBJ := $(SRC:src/%.c=obj/%.o)

CC := cc

CFLAGS := -Wall -Wextra -Werror -Isrc -MMD -MP $(CF)
LDFLAGS := -lreadline $(LDF)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

obj:
	mkdir -p obj

obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean
	+$(MAKE) all

-include $(OBJ:.o=.d)
