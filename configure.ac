AC_INIT([42sh], [1.0], [thibaut.dupre@epita.fr, baptsite.faroy@epita.fr,antonin.maitre@epita.fr, paul.norais@epita.fr])
AM_INIT_AUTOMAKE([subdir-objects] [foreign])

# Pretty display of Makefile rules
AM_SILENT_RULES([yes])

# Check if a C compiler is available
AC_PROG_CC
AC_PROG_RANLIB

AC_CONFIG_SRCDIR([src/42sh.c])

CFLAGS="${CFLAGS} -std=c99 -pedantic -Werror -Wall -Wextra -Wvla"


# Test for criterion package
PKG_CHECK_MODULES([CRITERION], [criterion >= 2.3.0], [have_criterion=yes], [have_criterion=no])
AM_CONDITIONAL([HAVE_CRITERION], [test "x$have_criterion" = xyes])

AC_CONFIG_FILES([
                    Makefile
                    src/Makefile
                    src/ast/Makefile
					src/lexer/Makefile
                    src/parser/Makefile
                    src/execution/Makefile
                    src/io/Makefile
                    src/builtins/Makefile
                    tests/Makefile
                ])
AC_OUTPUT