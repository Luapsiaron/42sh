#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "token/token.h"

enum lexer_conditon
{
    LEXER_FORCE_WORD, // force next token to be a word
    LEXER_NORMAL, // normal lexing, can be reserved words or operators
    LEXER_WORD_UNTIL, // word next = WORD until separator or operator
};

// Read characters frome a file and return tokens
// One by one for the parser
struct lexer
{
    FILE *input;
    int current;
    enum lexer_conditon condition;
    int error;
};

int lexer_error_occured(const struct lexer *lx);

// Initialize the lexer struct and read first character
void lexer_init(struct lexer *lexer, FILE *input);

// Return next token from input
struct token *lexer_next(struct lexer *lexer);

#endif /* ! LEXER_H */
