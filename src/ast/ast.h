#ifndef AST_H
#define AST_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/str/str.h"

/* transform tokens into AST nodes */

// !! Variables / Pipe / Redirect for STEP 2 !!

// input = list '\n' | list EOF | '\n' | EOF;
// list = and_or;
// and_or = pipeline;
// pipeline = command;
// command = simple_command;
// simple_command = WORD { element };
// element = WORD;

enum ast_type
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
    AST_REDIR,
    AST_ASSIGNMENT
};

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

enum redir_type
{
    REDIR_OUT, // >
    REDIR_APPEND, // >>
    REDIR_CLOBBER, // >|
    REDIR_IN, // <
};

struct ast_redir
{
    int io_number;
    enum redir_type type; // IN/OUT/APPEND/CLOBBER
    char *word; // filename
    struct ast *next;
};

struct ast_assignment
{
    char *var_name;
    char *value;
    struct ast *next;
};

struct ast_cmd
{
    char **argv; // list of args
    struct ast *redirs;
    struct ast *assignments;
};

struct ast_negation
{
    struct ast *child;
};

enum and_or_op
{
    AND_OP, // &&
    OR_OP // ||
};

struct ast_and_or
{
    enum and_or_op operator;
    struct ast *left;
    struct ast *right;
};

enum loop_type
{
    LOOP_WHILE,
    LOOP_UNTIL
};

struct ast_while_until
{
    enum loop_type type; // WHILE/UNTIL
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
    struct ast_assignment ast_assignment;
};

struct ast
{
    enum ast_type type;
    union ast_union data;
};

struct ast *ast_init(enum ast_type type);

struct ast *ast_if_init(struct ast *condition, struct ast *then_body, struct ast *else_body);
struct ast *ast_cmd_init(char **argv);
struct ast *ast_list_init(struct ast *next, struct ast *child);
struct ast *ast_pipeline_init(struct ast *right, struct ast *left);
struct ast *ast_negation_init(struct ast *child);
struct ast *ast_and_or_init(enum and_or_op operator, struct ast * left, struct ast *right);
struct ast *ast_while_until_init(struct ast *condition, struct ast *body, enum loop_type type);
struct ast *ast_for_init(struct ast *first_arg, struct ast *second_arg, struct ast *body);

struct ast *ast_redir_init(int io_number, enum redir_type type, const char *word,
                      struct ast *next);
int ast_redir_append(struct ast *cmd, struct ast *redir);

struct ast *ast_assignment_init(const char *var, const char *value);
int ast_assignment_append(struct ast *cmd, struct ast *assignment);

void free_argv(char **argv);
void ast_free(struct ast *node);

#endif /* ! AST_H */
