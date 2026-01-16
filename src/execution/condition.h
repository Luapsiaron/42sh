#ifndef CONDITION_H
#define CONDITION_H

#include "../ast/ast.h"
#include "../expansion/hashmap.h"

int eval_condition(ast_t *condition, struct hash_map *hm);

#endif /* ! CONDITION_H */