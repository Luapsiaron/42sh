#include <stdlib.h>
#include <string.h>

#include "utils/str/str.h"

char *xstrdup(const char *s)
{
    if (!s)
        return NULL;

    size_t n = strlen(s) + 1;
    char *p = malloc(n);
    if (!p)
        return NULL;

    memcpy(p, s, n);
    return p;
}
