#ifndef EXECUTION_H
#define EXECUTION_H

#include <sys/types.h>

#include "../expansion/hashmap.h"
#include "ast/ast.h"

int exec_cmd_node(struct ast *cmd, struct hash_map *hm);
int exec_ast(struct ast *ast, struct hash_map *hm);
int exec_list(struct ast *ast, struct hash_map *hm);
int exec_if(struct ast *ast, struct hash_map *hm);
int exec_pipeline(struct ast *pipe_node, struct hash_map *hm);
int wait_status(pid_t pid);
int child_exec_command(struct ast *node, struct hash_map *hm);

#endif /* ! EXECUTION_H */
