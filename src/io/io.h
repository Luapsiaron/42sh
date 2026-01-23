#ifndef IO_H
#define IO_H

#include <stdio.h>

// Take a string and return a temporary file with the string in it
FILE *io_string_to_file(const char *string);

#endif /* ! IO_H */
