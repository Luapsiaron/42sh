#ifndef EXECUTION_H
#define EXECUTION_H

#include "ast/ast.h"

int exec_cmd(char **argv);
int exec_ast(ast_t *ast);
int exec_list(ast_t *ast);
int exec_if(ast_t *ast);

#endif /* ! EXECUTION_H */
