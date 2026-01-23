#include <stdlib.h>

#include "ast.h"

/*
    All the free functions for each AST node type
*/
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

static void free_cmd(struct ast *node)
{
    free_argv(node->data.ast_cmd.argv);
    ast_free(node->data.ast_cmd.redirs);
    ast_free(node->data.ast_cmd.assignments);
}

static void free_redir(struct ast *node)
{
    free(node->data.ast_redir.word);
    ast_free(node->data.ast_redir.next);
}

static void free_assignment(struct ast *node)
{
    free(node->data.ast_assignment.var_name);
    free(node->data.ast_assignment.value);
    ast_free(node->data.ast_assignment.next);
}

static void free_if(struct ast *node)
{
    ast_free(node->data.ast_if.condition);
    ast_free(node->data.ast_if.then_body);
    ast_free(node->data.ast_if.else_body);
}

static void free_list(struct ast *node)
{
    ast_free(node->data.ast_list.next);
    ast_free(node->data.ast_list.child);
}

static void free_pipeline(struct ast *node)
{
    ast_free(node->data.ast_pipeline.right);
    ast_free(node->data.ast_pipeline.left);
}

static void free_negation(struct ast *node)
{
    ast_free(node->data.ast_negation.child);
}

static void free_and_or(struct ast *node)
{
    ast_free(node->data.ast_and_or.left);
    ast_free(node->data.ast_and_or.right);
}

static void free_while_until(struct ast *node)
{
    ast_free(node->data.ast_while_until.condition);
    ast_free(node->data.ast_while_until.body);
}

static void free_for(struct ast *node)
{
    ast_free(node->data.ast_for.first_arg);
    ast_free(node->data.ast_for.second_arg);
    ast_free(node->data.ast_for.body);
}

static void free_block(struct ast *node)
{
    ast_free(node->data.ast_block.body);
}

static void free_funcdec(struct ast *node)
{
    free(node->data.ast_funcdec.name);
    ast_free(node->data.ast_funcdec.body);
    ast_free(node->data.ast_funcdec.redirs);
}

static void free_redirwrap(struct ast *node)
{
    ast_free(node->data.ast_redirwrap.shell_command);
    ast_free(node->data.ast_redirwrap.redirections);
}

typedef void (*free_func_t)(struct ast *node);

static void free_noop(struct ast *node)
{
    (void)node;
}

static const free_func_t free_funcs[] = {
    [AST_CMD] = free_cmd,
    [AST_REDIR] = free_redir,
    [AST_ASSIGNMENT] = free_assignment,
    [AST_IF] = free_if,
    [AST_LIST] = free_list,
    [AST_PIPELINE] = free_pipeline,
    [AST_NEGATION] = free_negation,
    [AST_AND_OR] = free_and_or,
    [AST_WHILE_UNTIL] = free_while_until,
    [AST_FOR] = free_for,
    [AST_BLOCK] = free_block,
    [AST_FUNCDEC] = free_funcdec,
    [AST_REDIRWRAP] = free_redirwrap,
};

static free_func_t get_free_func(enum ast_type type)
{
    size_t n = sizeof(free_funcs) / sizeof(free_funcs[0]);
    if ((size_t)type < n && free_funcs[type])
    {
        return free_funcs[type];
    }
    return free_noop;
}
void ast_free(struct ast *node)
{
    if (!node)
    {
        return;
    }
    get_free_func(node->type)(node);
    free(node);
}

struct ast *ast_init(enum ast_type type)
{
    struct ast *new = calloc(1, sizeof(*new));
    if (!new)
    {
        return NULL;
    }

    new->type = type;
    return new;
}