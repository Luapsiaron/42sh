#include "lexer.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/stack/stack.h"

//static struct stack *stack;

static void lexer_next_char(lexer_t *lx)
{
    lx->current = fgetc(lx->input);
}

static token_t *lexer_is_word(lexer_t *lx)
{
    char buffer[512];
    size_t i = 0;

    while (lx->current != EOF && lx->current != ';' && lx->current != '|'
           && lx->current != '>' && lx->current != '\n'
           && !isspace(lx->current))
    {
        buffer[i++] = lx->current;
        lexer_next_char(lx);
    }
    buffer[i] = '\0';

    token_type_t type;
    if (token_is_reserved_word(buffer, &type))
    {
        return token_new(type, NULL);
    }
    return token_new(TOKEN_WORD, buffer);
}

void lexer_init(lexer_t *lx, FILE *input)
{
    lx->input = input;
    lx->current = fgetc(input);
}

token_t *lexer_next(lexer_t *lx)
{
    while (lx->current == ' ')
    {
        lexer_next_char(lx);
    }

    if (lx->current == ';')
    {
        lexer_next_char(lx);
        return token_new(TOKEN_SEMICOLON, NULL);
    }
    if (lx->current == '\n')
    {
        lexer_next_char(lx);
        return token_new(TOKEN_NEWLINE, NULL);
    }
    if (lx->current == '|')
    {
        lexer_next_char(lx);
        return token_new(TOKEN_PIPE, NULL);
    }
    if (lx->current == '>')
    {
        lexer_next_char(lx);
        if (lx->current == '>')
        {
            lexer_next_char(lx);
        }
        return token_new(TOKEN_REDIRECT, NULL);
    }
    if (lx->current == EOF)
    {
        return token_new(TOKEN_EOF, NULL);
    }
    return lexer_is_word(lx);
}
