#define _POSIX_C_SOURCE 200809L

#include "io.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static char *io_buf = NULL;

FILE *
io_string_to_file(const char *string) // creates a temporary file from a string
{
    if (!string)
        return NULL;
    size_t len = strlen(string);
    free(io_buf);
    io_buf = malloc(len + 1);
    if (!io_buf)
        return NULL;
    memcpy(io_buf, string, len + 1);
    FILE *f = fmemopen(io_buf, len, "r");
    if (!f)
    {
        free(io_buf);
        io_buf = NULL;
        return NULL;
    }
    fseek(f, 0, SEEK_SET);
    return f;
}
