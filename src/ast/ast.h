#ifndef AST_H
#define AST_H

#include <stdio.h>

/* transform tokens into AST nodes */

// !! Variables / Pipe / Redirect for STEP 2 !!

// input = list '\n' | list EOF | '\n' | EOF;
// list = and_or;
// and_or = pipeline;
// pipeline = command;
// command = simple_command;
// simple_command = WORD { element };
// element = WORD;

typedef enum ast_type
{
    AST_INPUT,
    AST_LIST,
    AST_AND_OR,
    AST_PIPELINE,
    AST_CMD,
    AST_SIMPLE_CMD,
    AST_ELEMENT,
    AST_IF
} ast_type_t;

struct ast_if
{
    struct ast *condition; // the condition
    struct ast *then_body; // the body of the then clause
    struct ast *else_body; // the body of the else, may be NULL
};

struct ast_cmd // part with multiple commands
{
    char **argv; // list of args
}; // no need to have a size as argv finish with NULL

struct ast_input;

struct ast_list
{
    struct ast *next;
    struct ast *child;
};

struct ast_pipe
{
    struct ast *right;
    struct ast *left;
};

struct ast_redir
{
    struct ast *next;
    FILE *redirect;
};

union ast_union
{
    struct ast_cmd ast_cmd;
    struct ast_if ast_if;
    struct ast_list ast_list;
    struct ast_pipe ast_pipe;
    struct ast_redir ast_redir;
};

typedef struct ast
{
    ast_type_t type;
    union ast_union data;
} ast_t;

ast_t *ast_if_init(ast_t *condition, ast_t *then_body, ast_t *else_body);
ast_t *ast_cmd_init(char **argv);
ast_t *ast_list_init(ast_t *next, ast_t *child);
ast_t *ast_pipe_init(ast_t *right, ast_t *left);

void free_argv(char **argv);
void ast_free(ast_t *node);

void ast_printer(const ast_t *node, int depth);

#endif /* ! AST_H */
