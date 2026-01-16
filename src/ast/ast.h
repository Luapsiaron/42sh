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
    AST_IF,
    AST_WHILE_UNTIL,
    AST_FOR,
    AST_UNTIL,
    AST_NEGATION,
    AST_REDIR
} ast_type_t;

struct ast_if
{
    struct ast *condition; // the condition
    struct ast *then_body; // the body of the then clause
    struct ast *else_body; // the body of the else, may be NULL
};

struct ast_input;

struct ast_list
{
    struct ast *next;
    struct ast *child;
};

struct ast_pipeline
{
    struct ast *right;
    struct ast *left;
};

typedef enum redir_type
{
    REDIR_OUT, // >
    REDIR_APPEND, // >>
    REDIR_CLOBBER, // >|
    REDIR_IN, // <
} redir_type_t;

typedef struct ast_redir
{
    int io_number;
    redir_type_t type; // IN/OUT/APPEND/CLOBBER
    char *word; // filename
    struct ast *next;
} ast_redir_t;

struct ast_cmd
{
    char **argv; // list of args
    struct ast *redirs;
};

struct ast_negation
{
    struct ast *child;
};

typedef enum and_or_op
{
    AND_OP, // &&
    OR_OP // ||
} and_or_op_t;

struct ast_and_or
{
    and_or_op_t operator;
    struct ast *left;
    struct ast *right;
};

typedef enum loop_type
{
    LOOP_WHILE,
    LOOP_UNTIL
} loop_t;

struct ast_while_until
{
    loop_t type; // WHILE/UNTIL
    struct ast *condition;
    struct ast *body;
};

struct ast_for
{
    struct ast *first_arg;
    struct ast *second_arg;
    struct ast *body;
};

union ast_union
{
    struct ast_cmd ast_cmd;
    struct ast_if ast_if;
    struct ast_list ast_list;
    struct ast_pipeline ast_pipeline;
    struct ast_negation ast_negation;
    struct ast_and_or ast_and_or;
    struct ast_while_until ast_while_until;
    struct ast_for ast_for;
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
ast_t *ast_pipeline_init(ast_t *right, ast_t *left);
ast_t *ast_negation_init(ast_t *child);
ast_t *ast_and_or_init(and_or_op_t operator, ast_t * left, ast_t *right);
ast_t *ast_while_until_init(ast_t *condition, ast_t *body, loop_t type);
ast_t *ast_for_init(ast_t *first_arg, ast_t *second_arg, ast_t *body);

ast_t *ast_redir_init(int io_number, redir_type_t type, const char *word,
                      ast_t *next);
int ast_redir_append(ast_t *cmd, ast_t *redir);

#endif /* ! AST_H */
