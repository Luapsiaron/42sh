#include "parser_internal.h"

/*
    Parse logical AND/OR operations
    Grammar: and_or = pipeline { ( '&&' | '||' ) { '\n' } pipeline } ;

    1. Parse the left pipeline
    2. Folds successive AND/OR operations into a left-associative AST
    3. Builds an AST_AND_OR node for each operation

    Example: cmd1 && cmd2 || cmd3
    AST representation:
          OR_OP
         /     \
      AND_OP    cmd3
      /    \
   cmd1    cmd2
*/
struct ast *parse_and_or(struct parser *p)
{
    struct ast *left = parse_pipeline(p);
    if (!left)
    {
        return NULL;
    }

    while (peek(p) == TOKEN_AND_IF || peek(p) == TOKEN_OR_IF)
    {
        enum and_or_op operator;
        if (peek(p) == TOKEN_AND_IF)
        {
            operator= AND_OP;
        }
        else
        {
            operator= OR_OP;
        }
        pop(p);
        skip_newlines(p);

        struct ast *right = parse_pipeline(p);
        if (!right)
        {
            ast_free(left);
            return NULL;
        }

        struct ast *new_and_or = ast_and_or_init(operator, left, right);
        if (!new_and_or)
        {
            ast_free(left);
            ast_free(right);
            return NULL;
        }
        left = new_and_or;
    }

    return left;
}
