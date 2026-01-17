#include "ast.h"

ast_t *ast_cmd_init(char **argv)
{
    ast_t *new = ast_init(AST_CMD);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_cmd.argv = argv;
    new->data.ast_cmd.redirs = NULL;
    new->data.ast_cmd.assignments = NULL;

    return new;
}

ast_t *ast_list_init(ast_t *next, ast_t *child)
{
    ast_t *new = ast_init(AST_LIST);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_list.next = next;
    new->data.ast_list.child = child;

    return new;
}