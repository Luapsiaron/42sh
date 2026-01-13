#ifndef IO_H
#define IO_H

#include <stdio.h>

// Take a string and return a temporary file with the string in it
FILE *io_string_to_file(const char *string);

// Take standard inout to put it into a temporary file
FILE *io_stdin_to_file(void);

#endif /* ! IO_H */
