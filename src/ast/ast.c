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
        ast_free(node->data.ast_cmd.redirs);
        break;
    case AST_REDIR:
        free(node->data.ast_redir.word);
        ast_free(node->data.ast_redir.next);
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
    case AST_PIPELINE:
        ast_free(node->data.ast_pipeline.right);
        ast_free(node->data.ast_pipeline.left);
        break;
    case AST_NEGATION:
        ast_free(node->data.ast_negation.child);
        break;
    case AST_AND_OR:
        ast_free(node->data.ast_and_or.left);
        ast_free(node->data.ast_and_or.right);
        break;
    case AST_WHILE_UNTIL:
        ast_free(node->data.ast_while_until.condition);
        ast_free(node->data.ast_while_until.body);
        break;
    case AST_FOR:
        ast_free(node->data.ast_for.first_arg);
        ast_free(node->data.ast_for.second_arg);
        ast_free(node->data.ast_for.body);
        break;
    default:
        break;
    }
    free(node);
}

static const char *redir_name(redir_type_t type)
{
    switch (type)
    {
    case REDIR_IN:
        return "<";
    case REDIR_OUT:
        return ">";
    case REDIR_APPEND:
        return ">>";
    case REDIR_CLOBBER:
        return ">|";
    default:
        return "UNKNOWN";
    }
}

static void print_redirs(const ast_t *redir, int depth)
{
    while (redir)
    {
        for (int i = 0; i < depth; i++)
        {
            printf("  ");
        }
        printf("REDIR: %d%s %s\n", redir->data.ast_redir.io_number,
               redir_name(redir->data.ast_redir.type),
               redir->data.ast_redir.word);
        redir = redir->data.ast_redir.next;
    }
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
        if (node->data.ast_cmd.argv)
        {
            for (size_t i = 0; node->data.ast_cmd.argv[i]; i++)
            {
                printf("%s ", node->data.ast_cmd.argv[i]);
            }
        }
        printf("\n");
        print_redirs(node->data.ast_cmd.redirs, depth + 1);
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
    case AST_PIPELINE:
        printf("PIPELINE:\n");
        ast_printer(node->data.ast_pipeline.left, depth + 1);
        ast_printer(node->data.ast_pipeline.right, depth + 1);
        break;
    case AST_NEGATION:
        printf("NEGATION:\n");
        ast_printer(node->data.ast_negation.child, depth + 1);
        break;
    case AST_AND_OR:
        printf("AND_OR: %s\n",
               node->data.ast_and_or.operator== AND_OP ? "AND" : "OR");
        ast_printer(node->data.ast_and_or.left, depth + 1);
        ast_printer(node->data.ast_and_or.right, depth + 1);
        break;
    case AST_WHILE_UNTIL:
        if (node->data.ast_while_until.type == LOOP_WHILE)
        {
            printf("WHILE:\n");
        }
        else
        {
            printf("UNTIL:\n");
        }
        ast_printer(node->data.ast_while_until.condition, depth + 1);
        ast_printer(node->data.ast_while_until.body, depth + 1);
        break;
    case AST_FOR:
        printf("FOR:\n");
        ast_printer(node->data.ast_for.first_arg, depth + 1);
        ast_printer(node->data.ast_for.second_arg, depth + 1);
        ast_printer(node->data.ast_for.body, depth + 1);
        break;
    default:
        printf("Unknown AST node type\n");
        break;
    }
}
