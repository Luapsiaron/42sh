#include "parser_internal.h"

enum token_type peek(struct parser *p)
{
    if (!p || !p->current_token)
    {
        return TOKEN_EOF;
    }
    return p->current_token->type;
}

void pop(struct parser *p)
{
    if (!p)
    {
        return;
    }

    if (p->current_token)
    {
        token_free(p->current_token);
    }

    p->current_token = lexer_next(&p->lexer);
}

void skip_semicolon_newline(struct parser *p)
{
    while (peek(p) == TOKEN_NEWLINE || peek(p) == TOKEN_SEMICOLON)
    {
        pop(p);
    }
}

void skip_newlines(struct parser *p)
{
    while (peek(p) == TOKEN_NEWLINE)
    {
        pop(p);
    }
}

int remove_separator(struct parser *p)
{
    if (peek(p) == TOKEN_SEMICOLON)
    {
        pop(p);
        skip_newlines(p);
        return 1;
    }
    if (peek(p) == TOKEN_NEWLINE)
    {
        skip_newlines(p);
        return 1;
    }
    return 0;
}
