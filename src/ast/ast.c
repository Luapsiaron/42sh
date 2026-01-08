#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static ast_t *ast_init(ast_type_t type)
{
    ast_t *new = calloc(1, sizeof(*new));
    if (!new)
    {
        return NULL;
    }

    new->type = type;
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

ast_t *ast_cmd_init(char **argv)
{
    ast_t *new = ast_init(AST_CMD);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_cmd.argv = argv;

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

ast_t *ast_pipe_init(ast_t *right, ast_t *left)
{
    ast_t *new = ast_init(AST_PIPELINE);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_pipe.right = right;
    new->data.ast_pipe.left = left;

    return new;
}

static void argv_free(char **argv)
{
    if (!argv)
    {
        return;
    }

    for (size_t i = 0; argv[i]; i++)
    {
        free(argv[i]);
    }
    free(argv);
}

void ast_free(ast_t *node)
{
    if (!node)
    {
        return;
    }

    switch (node->type)
    {
    case AST_CMD:
        argv_free(node->data.ast_cmd.argv);
        break;

    case AST_IF:
        ast_free(node->data.ast_if.condition);
        ast_free(node->data.ast_if.then_body);
        ast_free(node->data.ast_if.else_body);
        break;

    case AST_LIST:
        ast_free(node->data.ast_list.next);
        ast_free(node->data.ast_list.child);
        break;
    default:
        break;
    }
    free(node);
}
