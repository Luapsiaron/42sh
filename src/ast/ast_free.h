#ifndef AST_FREE_H
#define AST_FREE_H

#include "ast.h"

void free_argv(char **argv);
void ast_free(ast_t *node);

#endif /* ! AST_FREE_H */