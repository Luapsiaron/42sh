#include "ast_free.h"

#include <stdlib.h>

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

static void free_cmd(ast_t *node)
{
    free_argv(node->data.ast_cmd.argv);
    ast_free(node->data.ast_cmd.redirs);
    ast_free(node->data.ast_cmd.assignments);
}

static void free_redir(ast_t *node)
{
    free(node->data.ast_redir.word);
    ast_free(node->data.ast_redir.next);
}

static void free_assignment(ast_t *node)
{
    free(node->data.ast_assignment.var_name);
    free(node->data.ast_assignment.value);
    ast_free(node->data.ast_assignment.next);
}

static void free_if(ast_t *node)
{
    ast_free(node->data.ast_if.condition);
    ast_free(node->data.ast_if.then_body);
    ast_free(node->data.ast_if.else_body);
}

static void free_list(ast_t *node)
{
    ast_free(node->data.ast_list.next);
    ast_free(node->data.ast_list.child);
}

static void free_pipeline(ast_t *node)
{
    ast_free(node->data.ast_pipeline.right);
    ast_free(node->data.ast_pipeline.left);
}

static void free_negation(ast_t *node)
{
    ast_free(node->data.ast_negation.child);
}

static void free_and_or(ast_t *node)
{
    ast_free(node->data.ast_and_or.left);
    ast_free(node->data.ast_and_or.right);
}

static void free_while_until(ast_t *node)
{
    ast_free(node->data.ast_while_until.condition);
    ast_free(node->data.ast_while_until.body);
}

static void free_for(ast_t *node)
{
    ast_free(node->data.ast_for.first_arg);
    ast_free(node->data.ast_for.second_arg);
    ast_free(node->data.ast_for.body);
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
        free_cmd(node);
        break;
    case AST_REDIR:
        free_redir(node);
        break;
    case AST_ASSIGNMENT:
        free_assignment(node);
        break;
    case AST_IF:
        free_if(node);
        break;
    case AST_LIST:
        free_list(node);
        break;
    case AST_PIPELINE:
        free_pipeline(node);
        break;
    case AST_NEGATION:
        free_negation(node);
        break;
    case AST_AND_OR:
        free_and_or(node);
        break;
    case AST_WHILE_UNTIL:
        free_while_until(node);
        break;
    case AST_FOR:
        free_for(node);
        break;
    default:
        break;
    }
    free(node);
}
