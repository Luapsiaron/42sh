#ifndef EXPAND_H
#define EXPAND_H

#include "hashmap.h"

struct exit_info
{
    int last;
};

typedef struct {
    char *buff;
    size_t idx;
    size_t capacity;
}buffer_t;

char **expand_argv(char **argv, struct hash_map *hm);
char *expand_word(char *word, struct hash_map *hm);

#endif /* ! EXPAND_H */