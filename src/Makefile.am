bin_PROGRAMS = 42sh

AM_CFLAGS   = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
AM_CPPFLAGS = -I$(top_srcdir)/src

noinst_LIBRARIES = lib42sh.a

lib42sh_a_SOURCES = \
  ast/ast.c \
  ast/ast_free.c \
  ast/ast_printer.c \
  builtins/echo.c \
  execution/execution.c \
  execution/redir_pipe.c \
  execution/loop.c \
  io/io.c \
  lexer/lexer.c \
  lexer/token/token.c \
  parser/parser.c \
  parser/parser_command.c \
  parser/parser_if.c \
  parser/parser_input.c \
  parser/parser_list.c \
  parser/parser_simple_command.c \
  parser/parser_pipeline.c \
  parser/parser_while_until.c \
  parser/parser_for.c \
  parser/parser_and_or.c \
  parser/parser_redir.c \
  parser/parser_assignment.c \
  utils/stack/stack.c \
  utils/str/str.c \
  utils/parser/pretty_printer.c

42sh_SOURCES = 42sh.c
42sh_LDADD   = lib42sh.a
