#ifndef HASHMAP_H
#define HASHMAP_H

#include <stddef.h>

struct pair_list
{
    const char *key;
    char *value;
    struct pair_list *next;
};

struct hash_map
{
    struct pair_list **data;
    size_t size;
};


#endif /* ! HASHMAP_H */