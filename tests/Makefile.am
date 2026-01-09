AM_CFLAGS   = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
AM_CPPFLAGS = -I$(top_srcdir)/src $(CRITERION_CFLAGS)

check_PROGRAMS = \
  test_echo \
  test_io_string_to_file \
  test_io_stdin_to_file

if HAVE_CRITERION

TESTS = run_tests.sh
EXTRA_DIST = run_tests.sh

endif


test_echo_SOURCES = unit/test_echo.c
test_echo_LDADD   = $(top_builddir)/src/lib42sh.a $(CRITERION_LIBS)

test_io_string_to_file_SOURCES = unit/test_io_string_to_file.c
test_io_string_to_file_LDADD   = $(top_builddir)/src/lib42sh.a $(CRITERION_LIBS)

test_io_stdin_to_file_SOURCES = unit/test_io_stdin_to_file.c
test_io_stdin_to_file_LDADD   = $(top_builddir)/src/lib42sh.a $(CRITERION_LIBS)
