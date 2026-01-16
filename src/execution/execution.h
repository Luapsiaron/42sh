#ifndef EXECUTION_H
#define EXECUTION_H

#include <sys/types.h>

#include "ast/ast.h"
#include "../expansion/hashmap.h"

int exec_cmd_node(ast_t *cmd, struct hash_map *hm);
int exec_ast(ast_t *ast, struct hash_map *hm);
int exec_list(ast_t *ast, struct hash_map *hm);
int exec_if(ast_t *ast, struct hash_map *hm);
int exec_pipeline(ast_t *pipe_node, struct hash_map *hm);
int wait_status(pid_t pid);
int child_exec_command(ast_t *node, struct hash_map *hm);

#endif /* ! EXECUTION_H */
