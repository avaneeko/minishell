# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jgueon <jgueon@student.hive.fi>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/07/24 17:33:26 by losypenk          #+#    #+#              #
#    Updated: 2025/10/16 21:25:10 by jgueon           ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

NAME := minishell

SRC := src/main.c src/utils.c src/utils2.c src/utils3.c src/token.c \
	src/token_list.c src/token_list2.c src/tokenizer.c src/app_create.c \
	src/app_destroy.c src/env.c src/env1.c src/env2.c src/expansion.c \
	src/astr.c src/token_resplit.c src/heredoc.c src/expansion_utils.c \
	src/heredoc_bad_good_heredoc.c src/heredoc_expand_prompt.c \
	src/heredoc_get_file_name.c src/heredoc_input.c src/heredoc_input1.c \
	src/heredoc_open_write.c src/cmdarr.c src/exec_builder.c \
	src/expand_last_err.c src/get_next_line.c src/get_next_line_utils.c \
	src/tokenizer1.c \
	src/token_resplit1.c \
	src/astr1.c \
	src/app_reset_exec.c \
	src/prompt.c \
	src/dequoter.c \
	src/exec_handle_redir.c \
	src/exec_builder1.c \
	src/syntax_checker.c \
	src/Builtins/builtin_cd.c \
	src/Builtins/builtin_echo.c \
	src/Builtins/builtin_env.c \
	src/Builtins/builtin_exit.c \
	src/Builtins/builtin_export.c \
	src/Builtins/builtin_pwd.c \
	src/Builtins/builtin_unset.c \
	src/Builtins/builtins_utils1.c \
	src/Builtins/builtins_utils2.c \
	src/Builtins/builtins_utils3.c \
	src/Builtins/builtins_utils4.c \
	src/Builtins/builtins_utils5.c \
	src/Builtins/builtins_utils6.c \
	src/Builtins/builtins_utils7.c \
	src/Builtins/builtins_utils8.c \
	src/Builtins/builtins_utils9.c \
	src/Builtins/env_helpers.c \
	src/Execution/command_path.c \
	src/Execution/execution_env.c \
	src/Execution/execution_exec.c \
	src/Execution/execution_resources.c \
	src/Execution/execution_utils.c \
	src/Execution/execution_utils2.c \
	src/Execution/execution_utils3.c \
	src/Execution/execution_utils4.c \
	src/Execution/execution_utils5.c \
	src/Execution/execution_wait.c \
	src/Execution/executor_helpers.c \
	src/Execution/executor.c \
	src/Execution/redirection.c \
	src/Signals/signals.c \
	src/Signals/signals1.c

OBJ := $(SRC:src/%.c=obj/%.o)

CC := cc

CFLAGS := -Wall -Wextra -Werror -Isrc -MMD -MP $(CF)
LDFLAGS := -lreadline $(LDF)

all: $(NAME)

$(NAME): $(OBJ)
	$(CC) $(LDFLAGS) $^ -o $@

obj:
	mkdir -p obj
	mkdir -p obj/Builtins/
	mkdir -p obj/Execution/
	mkdir -p obj/Signals/


obj/%.o: src/%.c | obj
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -rf obj

fclean: clean
	rm -f $(NAME)

re: fclean
	+$(MAKE) all

-include $(OBJ:.o=.d)
