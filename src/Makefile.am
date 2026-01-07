SUBDIRS = ast \
          execution \
          io \
          lexer \
          parser \
          builtins


bin_PROGRAMS = 42sh

noinst_LIBRARIES = libshell.a
libshell_a_SOURCES = 

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%

42sh_CFLAGS = -std=c99 -pedantic -Werror -Wall -Wextra -Wvla

42sh_LDADD = ast/libast.a \
            execution/libexecution.a \
            parser/libparser.a \
            lexer/liblexer.a \
            builtins/libbuiltins.a

			

