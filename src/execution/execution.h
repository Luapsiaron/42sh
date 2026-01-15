#ifndef EXECUTION_H
#define EXECUTION_H

#include <sys/types.h>

#include "ast/ast.h"

int exec_cmd_node(ast_t *cmd);
int exec_ast(ast_t *ast);
int exec_list(ast_t *ast);
int exec_if(ast_t *ast);
int exec_pipeline(ast_t *pipe_node);
int wait_status(pid_t pid);
int child_exec_command(ast_t *cmd);

#endif /* ! EXECUTION_H */
