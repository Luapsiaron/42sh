#include "lexer_internal.h"

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