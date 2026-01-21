noinst_LIBRARIES = libparser.a

AM_CFLAGS   = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
AM_CPPFLAGS = -I$(top_srcdir)/src

libparser_a_SOURCES = \
  parser.c \
  parser_command.c \
  parser_if.c \
  parser_input.c \
  parser_list.c \
  parser_simple_command.c \
  parser_pipeline.c \
  parser_while_until.c \
  parser_for.c \
  parser_and_or.c \
  parser_redir.c \
  parser_assignment.c \
  parser_shell_command.c \
  parser.h \
  parser_internal.h
