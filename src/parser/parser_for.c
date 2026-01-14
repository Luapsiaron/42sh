#include "parser_internal.h"

static const token_type_t END_TOKENS_DONE[] = { TOKEN_DONE };

static ast_t *parse_for_body(parser_t *p)
{
    return parse_compound_list(
        p, END_TOKENS_DONE, sizeof(END_TOKENS_DONE) / sizeof(*END_TOKENS_DONE));
}

static ast_t *parse_for_condition(parser_t *p, ast_t *second_arg)
{
    if (peek(p) == TOKEN_NEWLINE)
    {
        skip_newlines(p);
    }
    if (peek(p) != TOKEN_IN)
    {
        return NULL;
    }
    pop(p);
    second_arg = parse_simple_command(p);
    if (!remove_separator(p))
    {
        return NULL;
    }
    return second_arg;
}

ast_t *parse_for(parser_t *p)
{
    pop(p);

    ast_t *first_arg = parse_simple_command(p);
    ast_t *second_arg = NULL;
    if (!first_arg)
    {
        goto error;
    }

    skip_newlines(p);

    if (peek(p) != TOKEN_SEMICOLON)
    {
        second_arg = parse_for_condition(p, second_arg);
        if (!second_arg)
        {
            goto error;
        }
    }
    else
    {
        pop(p);
    }
    skip_newlines(p);

    if (peek(p) != TOKEN_DO)
    {
        goto error;
    }
    pop(p);

    ast_t *body = parse_for_body(p);
    if (!body)
    {
        goto error;
    }

    if (peek(p) != TOKEN_DONE)
    {
        goto error;
    }
    pop(p);

    return ast_for_init(first_arg, second_arg, body);

error:
    ast_free(first_arg);
    ast_free(second_arg);
    return NULL;
}
