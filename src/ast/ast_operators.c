#include "ast.h"

struct ast *ast_pipeline_init(struct ast *right, struct ast *left)
{
    struct ast *new = ast_init(AST_PIPELINE);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_pipeline.right = right;
    new->data.ast_pipeline.left = left;

    return new;
}

struct ast *ast_negation_init(struct ast *child)
{
    struct ast *new = ast_init(AST_NEGATION);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_negation.child = child;

    return new;
}

struct ast *ast_and_or_init(enum and_or_op operator, struct ast * left,
                            struct ast *right)
{
    struct ast *new = ast_init(AST_AND_OR);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_and_or.operator= operator;
    new->data.ast_and_or.left = left;
    new->data.ast_and_or.right = right;

    return new;
}