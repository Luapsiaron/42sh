#include "lexer_internal.h"

#include <stdio.h>

void lexer_next_char(struct lexer *lx) // advance to next character
{
    lx->current = fgetc(lx->input);
}

void skip_blanks(struct lexer *lx) // skip whitespace characters
{
    while (lx->current != EOF && isspace(lx->current) && lx->current != '\n')
    {
        lexer_next_char(lx);
    }
}

int lexer_peek_char(
    struct lexer *lx) // peek at next character without consuming it
{
    int c = fgetc(lx->input);
    if (c != EOF)
    {
        ungetc(c, lx->input);
    }
    return c;
}
