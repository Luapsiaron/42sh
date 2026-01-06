#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "token/token.h"

// Read characters frome a file and return tokens
// One by one for the parser
typedef struct lexer
{
    FILE *input;
    int current;
} lexer_t;

// Initialize the lexer struct and read first character
void lexer_init(lexer_t *lexer, FILE *input);

// Return next token from input
token_t *lexer_next(lexer_t *lexer);

#endif /* ! LEXER_H */
