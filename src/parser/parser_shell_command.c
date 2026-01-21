#include "parser_internal.h"

static const enum token_type END_TOKENS_BRACE[] = {TOKEN_RBRACE};
struct ast *parse_block(struct parser *p)
{
    if(peek(p) != TOKEN_LBRACE)
    {
        return NULL;
    }

    pop(p);
    skip_newlines(p);

    struct ast *body = parse_compound_list(p, END_TOKENS_BRACE, sizeof(END_TOKENS_BRACE) / sizeof(*END_TOKENS_BRACE));
    if(!body)
    {
        return NULL;
    }

    if(peek(p) != TOKEN_RBRACE)
    {
        ast_free(body);
        return NULL;
    }
    pop(p);
    return ast_block_init(body);
}