#include "ast.h"

struct ast *ast_block_init(struct ast *body)
{
    struct ast *new = ast_init(AST_BLOCK);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_block.body = body;
    return new;
}