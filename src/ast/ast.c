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

void free_argv(char **argv)
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
        free_argv(node->data.ast_cmd.argv);
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

void ast_printer(const ast_t *node, int depth)
{
    if (!node)
    {
        return;
    }

    for (int i = 0; i < depth; i++)
    {
        printf("  ");
    }

    switch (node->type)
    {
    case AST_CMD:
        printf("CMD: ");
        for (size_t i = 0; node->data.ast_cmd.argv[i]; i++)
        {
            printf("%s ", node->data.ast_cmd.argv[i]);
        }
        printf("\n");
        break;

    case AST_IF:
        printf("IF:\n");
        ast_printer(node->data.ast_if.condition, depth + 1);
        for (int i = 0; i < depth; i++)
        {
            printf("  ");
        }
        printf("THEN:\n");
        ast_printer(node->data.ast_if.then_body, depth + 1);
        if (node->data.ast_if.else_body)
        {
            for (int i = 0; i < depth; i++)
            {
                printf("  ");
            }
            printf("ELSE:\n");
            ast_printer(node->data.ast_if.else_body, depth + 1);
        }
        break;

    case AST_LIST:
        printf("LIST:\n");
        ast_printer(node->data.ast_list.child, depth + 1);
        ast_printer(node->data.ast_list.next, depth);
        break;

    default:
        printf("Unknown AST node type\n");
        break;
    }
}