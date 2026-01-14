AM_CFLAGS   = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
AM_CPPFLAGS = -I$(top_srcdir)/src $(CRITERION_CFLAGS)

check_PROGRAMS = \
  test_ast \
  test_builtins \
  test_io \
  test_lexer 

TESTS = run_tests.sh
EXTRA_DIST = run_tests.sh

test_ast_SOURCES = unit/test_ast.c
test_ast_LDADD   = $(top_builddir)/src/lib42sh.a $(CRITERION_LIBS)
test_ast_LDFLAGS = -lcriterion

test_builtins_SOURCES = unit/test_builtins.c
test_builtins_LDADD   = $(top_builddir)/src/lib42sh.a $(CRITERION_LIBS)
test_builtins_LDFLAGS = -lcriterion

test_io_SOURCES = unit/test_io.c
test_io_LDADD   = $(top_builddir)/src/lib42sh.a $(CRITERION_LIBS)
test_io_LDFLAGS = -lcriterion

test_lexer_SOURCES = unit/test_lexer.c
test_lexer_LDADD   = $(top_builddir)/src/lib42sh.a $(CRITERION_LIBS)
test_lexer_LDFLAGS = -lcriterion
