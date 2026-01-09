AC_PREREQ([2.69])
AC_INIT([42sh], [1.0], [thibaut.dupre@epita.fr, baptsite.faroy@epita.fr,antonin.maitre@epita.fr, paul.norais@epita.fr])

AM_INIT_AUTOMAKE([foreign subdir-objects])
AM_SILENT_RULES([yes])

AC_CONFIG_SRCDIR([src/42sh.c])
AC_CONFIG_HEADERS([config.h])

AC_PROG_CC
AC_PROG_RANLIB

PKG_CHECK_MODULES([CRITERION], [criterion],
                  [have_criterion=yes],
                  [have_criterion=no])

AM_CONDITIONAL([HAVE_CRITERION], [test "x$have_criterion" = "xyes"])

AC_CONFIG_FILES([
  Makefile
  src/Makefile
  tests/Makefile
])
AC_OUTPUT