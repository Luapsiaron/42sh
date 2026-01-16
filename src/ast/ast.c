#include "ast.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/str/str.h"

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

ast_t *ast_cmd_init(char **argv)
{
    ast_t *new = ast_init(AST_CMD);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_cmd.argv = argv;
    new->data.ast_cmd.redirs = NULL;

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

ast_t *ast_redir_init(int io_number, redir_type_t type, const char *word,
                      ast_t *next)
{
    ast_t *new = ast_init(AST_REDIR);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_redir.io_number = io_number;
    new->data.ast_redir.type = type;

    if (word)
    {
        new->data.ast_redir.word = xstrdup(word);
        if (!new->data.ast_redir.word)
        {
            free(new);
            return NULL;
        }
    }
    new->data.ast_redir.next = next;
    return new;
}

int ast_redir_append(ast_t *cmd, ast_t *redir)
{
    if (!cmd || cmd->type != AST_CMD || !redir || redir->type != AST_REDIR)
    {
        return 0;
    }

    ast_t **head = &cmd->data.ast_cmd.redirs;
    if (!*head)
    {
        *head = redir;
        return 1;
    }

    ast_t *cur = *head;
    while (cur->data.ast_redir.next)
    {
        cur = cur->data.ast_redir.next;
    }
    cur->data.ast_redir.next = redir;
    return 1;
}
