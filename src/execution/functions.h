#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "../ast/ast.h"
#include "../expansion/hashmap.h"

struct sh_function
{
    char *name;
    struct ast *body;
    struct ast *redirs;
    struct sh_function *next;
};

struct fn_call
{
    struct hash_map *hm;
    char **argv;
    struct ast *redirs;
};

int functions_register(const struct ast *funcdec);
const struct sh_function *functions_lookup(const char *name);
void functions_free_all(void);

int exec_function_call(const struct sh_function *fn, struct fn_call *call);

#endif
