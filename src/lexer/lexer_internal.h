#ifndef LEXER_INTERNAL_H
#define LEXER_INTERNAL_H

#include "lexer.h"
#include <stddef.h>

void lexer_next_char(struct lexer *lx);
void skip_blanks(struct lexer *lx);

int append_buffer(char *buffer, size_t *index, size_t capacity, int c);

int is_assignment_word(const char *s);

int lexer_single_quotes(struct lexer *lx, char *buffer, size_t *index, size_t capacity);
int lexer_double_quotes(struct lexer *lx, char *buffer, size_t *index, size_t capacity);

struct token *lexer_is_word(struct lexer *lx);

struct token *handle_comment(struct lexer *lx);
struct token *handle_separator(struct lexer *lx);
struct token *handle_redirection(struct lexer *lx);
struct token *handle_and_or(struct lexer *lx);
struct token *handle_pipe_or(struct lexer *lx);
struct token *handle_negation(struct lexer *lx);
struct token *lexer_ionumber(struct lexer *lx);

#endif /* ! LEXER_INTERNAL_H */