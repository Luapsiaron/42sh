#define _POSIX_C_SOURCE 200809L
#include "break.h"

#include <stdio.h>
#include <stdlib.h>

#include "../execution/execution.h"
#include "../expansion/hashmap.h"

static int argv_len(char **argv)
{
    int n = 0;
    while (argv && argv[n])
        n++;
    return n;
}

static int parse_n(const char *s, int *out) // parse 
{
    char *end = NULL;
    long v = strtol(s, &end, 10);
    if (!s || *s == '\0' || (end && *end != '\0'))
        return 1;
    if (v <= 0 || v > RET_BREAK_MAX)
        return 1;
    *out = (int)v;
    return 0;
}

static int loop_depth(struct hash_map *hm)
{
    char *v = hash_map_get(hm, "loop_depth");
    return v ? atoi(v) : 0;
}

static int break_code(int n)
{
    return RET_BREAK_BASE + (n - 1);
}

int builtin_break(char **argv, struct hash_map *hm)
{
    int depth = loop_depth(hm);
    int argc = argv_len(argv);
    if (depth <= 0)
        return fprintf(stderr, "break: not in a loop\n"), 2;

    int n = 1;
    if (argc >= 2 && parse_n(argv[1], &n) != 0)
        return fprintf(stderr, "break: %s: numeric argument required\n", argv[1]), 2;
    if (argc >= 3)
        return fprintf(stderr, "break: too many arguments\n"), 2;

    if (n > depth)
        n = depth;
    return break_code(n);
}
