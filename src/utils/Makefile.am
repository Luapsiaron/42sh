SUBDIRS = stack

lib_LIBRARIES = libutils.a

liblexer_a_SOURCES = utils.c \
  					utils.h

liblexer_a_CPPFLAGS = -I$(top_srcdir)/src

liblexer_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

noinst_LIBRARIES = libutils.a
