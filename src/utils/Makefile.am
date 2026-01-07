SUBDIRS = stack

lib_LIBRARIES = libutils.a

libutils_a_SOURCES = utils.c \
  					utils.h

libutils_a_CPPFLAGS = -I$(top_srcdir)/src

libutils_a_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

noinst_LIBRARIES = libutils.a
