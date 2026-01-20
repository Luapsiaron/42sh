#include "parser_internal.h"

struct ast *parse_pipeline(struct parser *p)
{
    int neg = 0;
    if (peek(p) == TOKEN_NEGATION)
    {
        pop(p);
        skip_newlines(p);
        neg = 1;
    }

    struct ast *left = parse_command(p);
    if (!left)
    {
        return NULL;
    }

    while (peek(p) == TOKEN_PIPE)
    {
        pop(p);
        skip_newlines(p);

        struct ast *right = parse_command(p);
        if (!right)
        {
            ast_free(left);
            return NULL;
        }

        struct ast *pipeline = ast_pipeline_init(right, left);
        if (!pipeline)
        {
            ast_free(left);
            ast_free(right);
            return NULL;
        }

        left = pipeline;
    }

    if (neg)
    {
        struct ast *negation = ast_negation_init(left);
        if (!negation)
        {
            ast_free(left);
            return NULL;
        }
        return negation;
    }

    return left;
}
