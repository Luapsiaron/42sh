SUBDIRS = ast builtins execution expansion io lexer parser utils

bin_PROGRAMS = 42sh

AM_CFLAGS   = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
AM_CPPFLAGS = -I$(top_srcdir)/src

42sh_SOURCES = 42sh.c

42sh_LDADD = \
  execution/libexecution.a \
  builtins/libbuiltins.a \
  io/libio.a \
  expansion/libexpand.a \
  parser/libparser.a \
  ast/libast.a \
  lexer/liblexer.a \
  utils/parser/libpretty_printer.a \
  utils/stack/libstack.a \
  utils/str/libstr.a

