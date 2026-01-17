#include "ast.h"

ast_t *ast_pipeline_init(ast_t *right, ast_t *left)
{
    ast_t *new = ast_init(AST_PIPELINE);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_pipeline.right = right;
    new->data.ast_pipeline.left = left;

    return new;
}

ast_t *ast_negation_init(ast_t *child)
{
    ast_t *new = ast_init(AST_NEGATION);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_negation.child = child;

    return new;
}

ast_t *ast_and_or_init(and_or_op_t operator, ast_t * left, ast_t *right)
{
    ast_t *new = ast_init(AST_AND_OR);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_and_or.operator= operator;
    new->data.ast_and_or.left = left;
    new->data.ast_and_or.right = right;

    return new;
}