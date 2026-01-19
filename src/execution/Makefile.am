noinst_LIBRARIES = libexecution.a

AM_CFLAGS   = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
AM_CPPFLAGS = -I$(top_srcdir)/src

libexecution_a_SOURCES = \
  execution.c \
  redir_pipe.c \
  loop.c \
  condition.c \
  execution.h \
  redir_pipe.h \
  loop.h \
  condition.h
