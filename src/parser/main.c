#include <stdio.h>

#include "parser.h"
#include "../io/io.h"
#include <stdio.h>

static FILE *get_file(FILE *f, char *string)
{
    if (!f)
    {
        if (!string)
        {
            f = io_stdin_to_file();
        }
        else
        {
            f = fopen(string, "r");//f = io_string_to_file(string);
        }
    }
    else
    {
        f = fopen(string, "r");
    }
    return f;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        return 0;
    }
    FILE *f = get_file(NULL, argv[1]);
    fseek(f, 0, SEEK_SET);
    ast_t *ast = parse_input(f);
    if(!ast)
    {
        fprintf(stderr, "Parsing error\n");
        return 1;
    }
    ast_printer(ast, 0);
    ast_free(ast);
    return 0;
}