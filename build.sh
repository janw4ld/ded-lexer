#!/bin/sh

set -xe

CC="gcc"
CFLAGS="-Wall -Wextra -std=c11 -pedantic"
LIBS=-lm
SRC="main.c common.c lexer.c"

$CC $CFLAGS -o lexer $SRC $LIBS
