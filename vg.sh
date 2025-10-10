#!/bin/sh

make CF='-glldb' CC=clang -Wno-error -j12 &&
valgrind --leak-check=full --show-leak-kinds=all --track-origins=yes \
	--track-fds=yes -s --suppressions=vg.supp ./minishell
