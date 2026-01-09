#include "io.h"

#include <stdio.h>
#include <string.h>
#include <unistd.h>

FILE *io_string_to_file(char *string)
{
    FILE *temporary_file = tmpfile();
    if (!temporary_file)
    {
        return NULL;
    }

    fprintf(temporary_file, "%s", string);

    fseek(temporary_file, 0, SEEK_SET);
    return temporary_file;
}

FILE *io_stdin_to_file(void)
{
    FILE *temporary_file = tmpfile();
    if (!temporary_file)
    {
        return NULL;
    }

    char buffer[4096];
    ssize_t size_readed;
    while ((size_readed = read(STDIN_FILENO, &buffer, sizeof(buffer))) > 0)
    {
        fprintf(temporary_file, "%s", buffer);
    }

    return temporary_file;
}
