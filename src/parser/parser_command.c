#include "parser_internal.h"

/*
    Parse a single command
    Grammar: command = simple_command | if_command | while_command | for_command | until_command

    Examples:
    - if true; then echo A; fi -> TOKEN_IF -> parse_if
    - while false; do echo B; done -> TOKEN_WHILE -> parse_while
    - A=1 echo hello -> TOKEN_ASSIGNMENT_WORD -> parse_simple_command
    - >out echo test -> TOKEN_GREAT -> parse_simple_command 
*/
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
