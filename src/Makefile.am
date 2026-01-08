bin_PROGRAMS = 42sh

AM_CFLAGS   = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
AM_CPPFLAGS = -I$(top_srcdir)/src

noinst_LIBRARIES = lib42sh.a

lib42sh_a_SOURCES = \
  ast/ast.c \
  builtins/echo.c \
  execution/execution.c \
  io/io.c \
  lexer/lexer.c \
  lexer/token/token.c \
  parser/parser.c \
  utils/stack/stack.c

42sh_SOURCES = 42sh.c
42sh_LDADD   = lib42sh.a
