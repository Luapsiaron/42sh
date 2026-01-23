#include "ast.h"

struct ast *ast_while_until_init(struct ast *condition, struct ast *body,
                                 enum loop_type type)
{
    struct ast *new = ast_init(AST_WHILE_UNTIL);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_while_until.condition = condition;
    new->data.ast_while_until.body = body;
    new->data.ast_while_until.type = type;

    return new;
}

struct ast *ast_for_init(struct ast *first_arg, struct ast *second_arg,
                         struct ast *body)
{
    struct ast *new = ast_init(AST_FOR);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_for.first_arg = first_arg;
    new->data.ast_for.second_arg = second_arg;
    new->data.ast_for.body = body;

    return new;
}

struct ast *ast_if_init(struct ast *condition, struct ast *then_body,
                        struct ast *else_body)
{
    struct ast *new = ast_init(AST_IF);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_if.condition = condition;
    new->data.ast_if.then_body = then_body;
    new->data.ast_if.else_body = else_body;

    return new;
}