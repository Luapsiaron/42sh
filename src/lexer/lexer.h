#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

#include "token/token.h"

typedef enum lexer_conditon
{
    LEXER_FORCE_WORD, // force next token to be a word
    LEXER_NORMAL, // normal lexing, can be reserved words or operators
    LEXER_WORD_UNTIL, // word next = WORD until separator or operator
} lexer_condition_t;

// Read characters frome a file and return tokens
// One by one for the parser
typedef struct lexer
{
    FILE *input;
    int current;
    lexer_condition_t condition;
} lexer_t;

// Initialize the lexer struct and read first character
void lexer_init(lexer_t *lexer, FILE *input);

// Return next token from input
token_t *lexer_next(lexer_t *lexer);

#endif /* ! LEXER_H */
