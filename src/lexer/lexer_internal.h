#ifndef LEXER_INTERNAL_H
#define LEXER_INTERNAL_H

#include "lexer.h"

#include <stddef.h>
#include <ctype.h>

void lexer_next_char(struct lexer *lx);
void skip_blanks(struct lexer *lx);

struct token *handle_comment(struct lexer *lx);
struct token *handle_separator(struct lexer *lx);
struct token *handle_redirection(struct lexer *lx);
struct token *handle_and_or(struct lexer *lx);
struct token *handle_pipe_or(struct lexer *lx);
struct token *handle_negation(struct lexer *lx);
struct token *lexer_ionumber(struct lexer *lx);

struct token *lexer_is_word(struct lexer *lx);

int lexer_peek_char(struct lexer *lx);

#endif /* ! LEXER_INTERNAL_H */