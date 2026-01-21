noinst_LIBRARIES = libast.a

AM_CFLAGS   = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
AM_CPPFLAGS = -I$(top_srcdir)/src

libast_a_SOURCES = \
  ast_commands.c \
  ast_basics.c \
  ast_core.c \
  ast_operators.c \
  ast_redir.c \
  ast_assignment.c \
  ast_printer.c \
  ast_block.c \
  ast.h \
  ast_printer.h
