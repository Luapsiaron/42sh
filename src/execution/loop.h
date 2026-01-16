#ifndef LOOP_H
#define LOOP_H

#include "../ast/ast.h"
#include "../expansion/hashmap.h"

int exec_while_until(ast_t *node, struct hash_map *hm);
int exec_for(ast_t *node, struct hash_map *hm);

#endif /* LOOP_H */