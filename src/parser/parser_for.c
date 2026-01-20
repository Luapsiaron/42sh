#include "parser_internal.h"

static const enum token_type END_TOKENS_DONE[] = { TOKEN_DONE };

static struct ast *parse_for_body(struct parser *p)
{
    return parse_compound_list(
        p, END_TOKENS_DONE, sizeof(END_TOKENS_DONE) / sizeof(*END_TOKENS_DONE));
}

static struct ast *parse_for_condition(struct parser *p, struct ast *second_arg)
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

struct ast *parse_for(struct parser *p)
{
    pop(p);

    struct ast *first_arg = parse_simple_command(p);
    struct ast *second_arg = NULL;
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

    struct ast *body = parse_for_body(p);
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
