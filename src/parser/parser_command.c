#include "parser_internal.h"

struct ast *parse_command(struct parser *p)
{
    if (peek(p) == TOKEN_IF)
    {
        return parse_if(p);
    }
    if (peek(p) == TOKEN_WHILE)
    {
        return parse_while(p);
    }
    if (peek(p) == TOKEN_UNTIL)
    {
        return parse_until(p);
    }
    if (peek(p) == TOKEN_FOR)
    {
        return parse_for(p);
    }
    if (peek(p) == TOKEN_WORD || peek(p) == TOKEN_ASSIGNMENT_WORD
        || peek(p) == TOKEN_IONUMBER || is_redirection_token(peek(p)))
    {
        return parse_simple_command(p);
    }
    return NULL;
}
