#include "parser_internal.h"

ast_t *parse_and_or(parser_t *p)
{
    ast_t *left = parse_pipeline(p);
    if (!left)
    {
        return NULL;
    }

    while (peek(p) == TOKEN_AND_IF || peek(p) == TOKEN_OR_IF)
    {
        and_or_op_t operator;
        if (peek(p) == TOKEN_AND_IF)
        {
            operator = AND_OP;
        }
        else
        {
            operator = OR_OP;
        }
        pop(p);
        skip_newlines(p);

        ast_t *right = parse_pipeline(p);
        if (!right)
        {
            ast_free(left);
            return NULL;
        }

        ast_t *new_and_or = ast_and_or_init(operator, left, right);
        if (!new_and_or)
        {
            ast_free(left);
            ast_free(right);
            return NULL;
        }
        left = new_and_or;
    }

    return left;
}