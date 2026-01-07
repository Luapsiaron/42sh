TESTS = run_all_tests.sh #$(check_PROGRAMS)

if HAVE_CRITERION

check_PROGRAMS = \
    unit/test_echo \
    unit/test_io_stdin_to_file \
    unit/test_io_string_to_file

# Flags communs (à l'intérieur du if)
TEST_CFLAGS = $(CRITERION_CFLAGS) -I$(top_srcdir)/src
TEST_LDADD = \
    $(top_builddir)/src/builtins/libbuiltins.a \
    $(top_builddir)/src/ast/libast.a \
    $(top_builddir)/src/execution/libexecution.a \
    $(top_builddir)/src/io/libio.a \
    $(top_builddir)/src/lexer/liblexer.a \
    $(top_builddir)/src/parser/libparser.a \
    $(CRITERION_LIBS)


unit_test_echo_SOURCES = unit/test_echo.c
unit_test_echo_CFLAGS = $(TEST_CFLAGS)
unit_test_echo_LDFLAGS = $(TEST_LDFLAGS)
unit_test_echo_LDADD = $(TEST_LDADD)

unit_test_io_stdin_to_file_SOURCES = unit/test_io_stdin_to_file.c
unit_test_io_stdin_to_file_CFLAGS = $(TEST_CFLAGS)
unit_test_io_stdin_to_file_LDFLAGS = $(TEST_LDFLAGS)
unit_test_io_stdin_to_file_LDADD = $(TEST_LDADD)

unit_test_io_string_to_file_SOURCES = unit/test_io_string_to_file.c
unit_test_io_string_to_file_CFLAGS = $(TEST_CFLAGS)
unit_test_io_string_to_file_LDFLAGS = $(TEST_LDFLAGS)
unit_test_io_string_to_file_LDADD = $(TEST_LDADD)

endif

EXTRA_DIST = \
    run_all_tests.sh \
    run_tests.sh runner.py \
    step1_simple/test_echo.py

clean-local:
	rm -rf .venv __pycache__ .pytest_cache step*/__pycache__