#include "parser_internal.h"

static const token_type_t END_TOKENS_DONE[] = { TOKEN_DONE };
static const token_type_t END_TOKENS_DO[] = { TOKEN_DO };

static ast_t *parse_while_body(parser_t *p)
{
    return parse_compound_list(
        p, END_TOKENS_DONE, sizeof(END_TOKENS_DONE) / sizeof(*END_TOKENS_DONE));
}

static ast_t *parse_while_condition(parser_t *p)
{
    return parse_compound_list(p, END_TOKENS_DO,
                               sizeof(END_TOKENS_DO) / sizeof(*END_TOKENS_DO));
}

ast_t *parse_while(parser_t *p)
{
    pop(p);

    ast_t *condition = parse_while_condition(p);
    if (!condition)
    {
        return NULL;
    }

    skip_newlines(p);

    if (peek(p) != TOKEN_DO)
    {
        ast_free(condition);
        return NULL;
    }
    pop(p);

    ast_t *body = parse_while_body(p);
    if (!body)
    {
        ast_free(condition);
        return NULL;
    }

    if (peek(p) != TOKEN_DONE)
    {
        ast_free(condition);
        ast_free(body);
        return NULL;
    }
    pop(p);

    return ast_while_init(condition, body);
}
