#include "ast.h"

ast_t *ast_while_until_init(ast_t *condition, ast_t *body, loop_t type)
{
    ast_t *new = ast_init(AST_WHILE_UNTIL);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_while_until.condition = condition;
    new->data.ast_while_until.body = body;
    new->data.ast_while_until.type = type;

    return new;
}

ast_t *ast_for_init(ast_t *first_arg, ast_t *second_arg, ast_t *body)
{
    ast_t *new = ast_init(AST_FOR);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_for.first_arg = first_arg;
    new->data.ast_for.second_arg = second_arg;
    new->data.ast_for.body = body;

    return new;
}

ast_t *ast_if_init(ast_t *condition, ast_t *then_body, ast_t *else_body)
{
    ast_t *new = ast_init(AST_IF);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_if.condition = condition;
    new->data.ast_if.then_body = then_body;
    new->data.ast_if.else_body = else_body;

    return new;
}