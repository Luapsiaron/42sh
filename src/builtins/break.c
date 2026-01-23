#define _POSIX_C_SOURCE 200809L
#include "break.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../execution/execution.h"
#include "../expansion/hashmap.h"
#include "../utils/str/str.h"

static int parse_n(char *s, int *out)
{
    char *end = NULL;
    long v = strtol(s, &end, 10);
    if (!s || *s == '\0' || (end && *end != '\0'))
        return 1;
    if (v <= 0 || v > 20)
        return 1;
    *out = (int)v;
    return 0;
}

static int get_loop_depth(struct hash_map *hm)
{
    char *v = hash_map_get(hm, "__loop_depth");
    return v ? atoi(v) : 0;
}

static int break_ret(int n)
{
    return RET_BREAK_BASE + (n - 1);
}

int builtin_break(char **argv, struct hash_map *hm)
{
    int depth = get_loop_depth(hm);
    if (depth <= 0)
        return fprintf(stderr, "break: not in a loop\n"), 2;

    int n = 1;
    if (argv[1] && parse_n(argv[1], &n) != 0)
        return fprintf(stderr, "break: %s: numeric argument required\n",
                       argv[1]),
               2;
    if (argv[2])
        return fprintf(stderr, "break: too many arguments\n"), 2;

    if (n > depth)
        n = depth;
    return break_ret(n);
}
