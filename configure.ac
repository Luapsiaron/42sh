AC_INIT([42sh], [1.0], [thibaut.dupre@epita.fr, baptsite.faroy@epita.fr,antonin.maitre@epita.fr, paul.norais@epita.fr])
AM_INIT_AUTOMAKE([subdir-objects] [foreign])

# Pretty display of Makefile rules
AM_SILENT_RULES([yes])

# Check if a C compiler is available
AC_PROG_CC
AC_PROG_RANLIB

AX_COMPILER_FLAGS([], [], [], [-std=c99 -pedantic -Werror -Wall -Wextra -Wvla])

AC_CONFIG_FILES([
                    Makefile
                    src/Makefile
                    src/ast/Makefile
					src/lexer/Makefile
                    src/parser/Makefile
                ])
AC_OUTPUT