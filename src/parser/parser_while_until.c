#include "parser_internal.h"

struct parser_while_until
{
    struct ast *condition;
    struct ast *body;
};

static const enum token_type END_TOKENS_DONE[] = { TOKEN_DONE };
static const enum token_type END_TOKENS_DO[] = { TOKEN_DO };

static struct ast *parse_body(struct parser *p)
{
    return parse_compound_list(
        p, END_TOKENS_DONE, sizeof(END_TOKENS_DONE) / sizeof(*END_TOKENS_DONE));
}

static struct ast *parse_condition(struct parser *p)
{
    return parse_compound_list(p, END_TOKENS_DO,
                               sizeof(END_TOKENS_DO) / sizeof(*END_TOKENS_DO));
}

static struct parser_while_until *
parse_while_until(struct parser *p, struct parser_while_until *res)
{
    pop(p);

    res->condition = parse_condition(p);
    if (!res->condition)
    {
        return NULL;
    }

    skip_newlines(p);

    if (peek(p) != TOKEN_DO)
    {
        goto error;
    }
    pop(p);

    res->body = parse_body(p);
    if (!res->body)
    {
        goto error;
    }

    if (peek(p) != TOKEN_DONE)
    {
        goto error;
    }
    pop(p);

    return res;

error:
    ast_free(res->condition);
    ast_free(res->body);
    res->condition = NULL;
    res->body = NULL;
    return NULL;
}

struct ast *parse_while(struct parser *p)
{
    struct parser_while_until res;

    if (!parse_while_until(p, &res))
    {
        return NULL;
    }
    return ast_while_until_init(res.condition, res.body, LOOP_WHILE);
}

struct ast *parse_until(struct parser *p)
{
    struct parser_while_until res;

    if (!parse_while_until(p, &res))
    {
        return NULL;
    }
    return ast_while_until_init(res.condition, res.body, LOOP_UNTIL);
}
