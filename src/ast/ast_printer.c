#include "ast_printer.h"

#include <stdio.h>

/*
    AST Printer for debugging purposes
*/

static void print_indent(int indent)
{
    for (int i = 0; i < indent; i++)
    {
        printf("  ");
    }
}

static const char *redir_name(enum redir_type type)
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

static void print_redirs(const struct ast *redir, int depth)
{
    while (redir)
    {
        print_indent(depth);
        printf("REDIR: %d%s %s\n", redir->data.ast_redir.io_number,
               redir_name(redir->data.ast_redir.type),
               redir->data.ast_redir.word);
        redir = redir->data.ast_redir.next;
    }
}

static void print_assignment(const struct ast *node, int depth)
{
    while (node)
    {
        print_indent(depth);
        printf("ASSIGNMENT: %s=%s\n", node->data.ast_assignment.var_name,
               node->data.ast_assignment.value);
        node = node->data.ast_assignment.next;
    }
}

static void print_block(const struct ast *node, int depth)
{
    print_indent(depth);
    printf("BLOCK:\n");
    ast_printer(node->data.ast_block.body, depth + 1);
}

static void print_cmd(const struct ast *node, int depth)
{
    print_indent(depth);
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
    print_assignment(node->data.ast_cmd.assignments, depth + 1);
}

static void print_if(const struct ast *node, int depth)
{
    print_indent(depth);
    printf("IF:\n");
    ast_printer(node->data.ast_if.condition, depth + 1);
    print_indent(depth);
    printf("THEN:\n");
    ast_printer(node->data.ast_if.then_body, depth + 1);
    if (node->data.ast_if.else_body)
    {
        print_indent(depth);
        printf("ELSE:\n");
        ast_printer(node->data.ast_if.else_body, depth + 1);
    }
}

static void print_list(const struct ast *node, int depth)
{
    print_indent(depth);
    printf("LIST:\n");
    ast_printer(node->data.ast_list.child, depth + 1);
    ast_printer(node->data.ast_list.next, depth);
}

static void print_pipeline(const struct ast *node, int depth)
{
    print_indent(depth);
    printf("PIPELINE:\n");
    ast_printer(node->data.ast_pipeline.left, depth + 1);
    ast_printer(node->data.ast_pipeline.right, depth + 1);
}

static void print_negation(const struct ast *node, int depth)
{
    print_indent(depth);
    printf("NEGATION:\n");
    ast_printer(node->data.ast_negation.child, depth + 1);
}

static void print_and_or(const struct ast *node, int depth)
{
    print_indent(depth);
    printf("AND_OR: %s\n",
           node->data.ast_and_or.operator== AND_OP ? "AND" : "OR");
    ast_printer(node->data.ast_and_or.left, depth + 1);
    ast_printer(node->data.ast_and_or.right, depth + 1);
}

static void print_while_until(const struct ast *node, int depth)
{
    print_indent(depth);
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
}

static void print_for(const struct ast *node, int depth)
{
    print_indent(depth);
    printf("FOR:\n");
    ast_printer(node->data.ast_for.first_arg, depth + 1);
    ast_printer(node->data.ast_for.second_arg, depth + 1);
    ast_printer(node->data.ast_for.body, depth + 1);
}

static void print_funcdec(const struct ast *node, int depth)
{
    print_indent(depth);
    printf("FUNCDEC: %s\n", node->data.ast_funcdec.name);
    print_indent(depth);
    printf("REDIRS:\n");
    print_redirs(node->data.ast_funcdec.redirs, depth + 1);
    print_indent(depth);
    printf("BODY:\n");
    ast_printer(node->data.ast_funcdec.body, depth + 1);
}

static void print_redirwrap(const struct ast *node, int depth)
{
    print_indent(depth);
    printf("REDIRWRAP:\n");
    print_indent(depth + 1);
    printf("SHELL_COMMAND:\n");
    ast_printer(node->data.ast_redirwrap.shell_command, depth + 2);
    print_indent(depth + 1);
    printf("REDIRECTIONS:\n");
    print_redirs(node->data.ast_redirwrap.redirections, depth + 2);
}

typedef void (*print_func_t)(const struct ast *node, int depth);

static void print_unknown(const struct ast *node, int depth)
{
    (void)node;
    print_indent(depth);
    printf("Unknown AST node type\n");
}

static const print_func_t print_funcs[] = {
    [AST_CMD] = print_cmd,
    [AST_BLOCK] = print_block,
    [AST_IF] = print_if,
    [AST_LIST] = print_list,
    [AST_PIPELINE] = print_pipeline,
    [AST_NEGATION] = print_negation,
    [AST_AND_OR] = print_and_or,
    [AST_WHILE_UNTIL] = print_while_until,
    [AST_FOR] = print_for,
    [AST_ASSIGNMENT] = print_assignment,
    [AST_FUNCDEC] = print_funcdec,
    [AST_REDIRWRAP] = print_redirwrap,
};

static print_func_t get_print_func(enum ast_type type)
{
    size_t n = sizeof(print_funcs) / sizeof(print_funcs[0]);
    if ((size_t)type < n && print_funcs[type])
    {
        return print_funcs[type];
    }
    return print_unknown;
}

void ast_printer(const struct ast *node, int depth)
{
    if (!node)
    {
        return;
    }

    get_print_func(node->type)(node, depth);
}
