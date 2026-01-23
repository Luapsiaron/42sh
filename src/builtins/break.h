#ifndef BREAK_H
#define BREAK_H

#include "../expansion/hashmap.h"

#define RET_BREAK_BASE 200
#define RET_BREAK_MAX 20

int builtin_break(char **argv, struct hash_map *hm);

#endif /* ! BREAK_H */
