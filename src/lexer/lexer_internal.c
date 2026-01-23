#include "lexer_internal.h"

#include <stdio.h>

void lexer_next_char(struct lexer *lx)
{
    lx->current = fgetc(lx->input);
}

void skip_blanks(struct lexer *lx)
{
    while (lx->current != EOF && isspace(lx->current) && lx->current != '\n')
    {
        lexer_next_char(lx);
    }
}

int lexer_peek_char(struct lexer *lx)
{
    int c = fgetc(lx->input);
    if (c != EOF)
    {
        ungetc(c, lx->input);
    }
    return c;
}