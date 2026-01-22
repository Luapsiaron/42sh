noinst_LIBRARIES = liblexer.a

AM_CFLAGS   = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
AM_CPPFLAGS = -I$(top_srcdir)/src

liblexer_a_SOURCES = \
  lexer.c \
  lexer_internal.c \
  lexer_ops.c \
  lexer_word.c \
  token/token.c \
  lexer.h \
  lexer_internal.h \
  token/token.h
