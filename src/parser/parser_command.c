#include "parser_internal.h"

ast_t *parse_command(parser_t *p)
{
    if (peek(p) == TOKEN_IF)
    {
        return parse_if(p);
    }
    if (peek(p) == TOKEN_WHILE)
    {
        return parse_while(p);
    }
    if (peek(p) == TOKEN_FOR)
    {
        return parse_for(p);
    }
    if (peek(p) == TOKEN_WORD)
    {
        return parse_simple_command(p);
    }
    return NULL;
}
