#include "ast.h"

struct ast *ast_cmd_init(char **argv)
{
    struct ast *new = ast_init(AST_CMD);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_cmd.argv = argv;
    new->data.ast_cmd.redirs = NULL;
    new->data.ast_cmd.assignments = NULL;

    return new;
}

struct ast *ast_list_init(struct ast *next, struct ast *child)
{
    struct ast *new = ast_init(AST_LIST);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_list.next = next;
    new->data.ast_list.child = child;

    return new;
}
