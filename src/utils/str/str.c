#include "str.h"

#include <stdlib.h>
#include <string.h>

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

char *xstrndup(const char *s, size_t n)
{
    if (!s)
        return NULL;

    size_t len = 0;
    while (len < n && s[len])
    {
        ++len;
    }

    char *p = malloc(len + 1);
    if (!p)
        return NULL;
    
    memcpy(p, s, len);
    p[len] = '\0';
    return p;
}
