AC_PREREQ([2.69])
AC_INIT([42sh], [1.0], [thibaut.dupre@epita.fr, baptsite.faroy@epita.fr,antonin.maitre@epita.fr, paul.norais@epita.fr])

AM_INIT_AUTOMAKE([foreign subdir-objects serial-tests])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src/42sh.c])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AC_PROG_RANLIB

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  src/ast/Makefile
  src/builtins/Makefile
  src/execution/Makefile
  src/expansion/Makefile
  src/io/Makefile
  src/lexer/Makefile
  src/parser/Makefile
  src/utils/Makefile
  src/utils/str/Makefile
  src/utils/stack/Makefile
  src/utils/parser/Makefile
  tests/Makefile
])
AC_OUTPUT