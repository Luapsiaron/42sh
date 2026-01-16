#include "parser_internal.h"

ast_t *parse_pipeline(parser_t *p)
{
    int neg = 0;
    if (peek(p) == TOKEN_NEGATION)
    {
        pop(p);
        skip_newlines(p);
        neg = 1;
    }

    ast_t *left = parse_command(p);
    if (!left)
    {
        return NULL;
    }

    while (peek(p) == TOKEN_PIPE)
    {
        pop(p);
        skip_newlines(p);

        ast_t *right = parse_command(p);
        if (!right)
        {
            ast_free(left);
            return NULL;
        }

        ast_t *pipeline = ast_pipeline_init(right, left);
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
        ast_t *negation = ast_negation_init(left);
        if (!negation)
        {
            ast_free(left);
            return NULL;
        }
        return negation;
    }

    return left;
}
