AM_CFLAGS   = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla
AM_CPPFLAGS = -I$(top_srcdir)/src $(CRITERION_CFLAGS)

check_PROGRAMS = \
  test_ast \
  test_builtins \
  test_io \
  test_lexer \
  test_execution \
  test_parser \
  test_ast_printer

TESTS = run_tests.sh
EXTRA_DIST = run_tests.sh

LIB42SH = \
  $(top_builddir)/src/execution/libexecution.a \
  $(top_builddir)/src/builtins/libbuiltins.a \
  $(top_builddir)/src/io/libio.a \
  $(top_builddir)/src/expansion/libexpand.a \
  $(top_builddir)/src/parser/libparser.a \
  $(top_builddir)/src/ast/libast.a \
  $(top_builddir)/src/lexer/liblexer.a \
  $(top_builddir)/src/utils/parser/libpretty_printer.a \
  $(top_builddir)/src/utils/str/libstr.a

test_ast_SOURCES = unit/test_ast.c
test_ast_LDADD   = $(LIB42SH) $(CRITERION_LIBS)
test_ast_LDFLAGS = -lcriterion

test_builtins_SOURCES = unit/test_builtins.c
test_builtins_LDADD   = $(LIB42SH) $(CRITERION_LIBS)
test_builtins_LDFLAGS = -lcriterion

test_io_SOURCES = unit/test_io.c
test_io_LDADD   = $(LIB42SH) $(CRITERION_LIBS)
test_io_LDFLAGS = -lcriterion

test_lexer_SOURCES = unit/test_lexer.c
test_lexer_LDADD   = $(LIB42SH) $(CRITERION_LIBS)
test_lexer_LDFLAGS = -lcriterion

test_parser_SOURCES = unit/test_parser.c
test_parser_LDADD   = $(LIB42SH) $(CRITERION_LIBS)
test_parser_LDFLAGS = -lcriterion

test_execution_SOURCES = unit/test_execution.c
test_execution_LDADD   = $(LIB42SH) $(CRITERION_LIBS)
test_execution_LDFLAGS = -lcriterion

test_ast_printer_SOURCES = unit/test_ast_printer.c
test_ast_printer_LDADD   = $(LIB42SH) $(CRITERION_LIBS)
test_ast_printer_LDFLAGS = -lcriterion
