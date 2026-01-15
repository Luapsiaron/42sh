#define _POSIX_C_SOURCE 200811L

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ast/ast.h"
#include "../io/io.h"
#include "../utils/parser/pretty_printer.h"
#include "parser.h"

static void usage(FILE *out)
{
    fprintf(out,
            "usage:\n"
            "  ./main FILE\n"
            "  ./main -c \"COMMAND\"\n"
            "  ./main --pretty FILE\n"
            "  ./main --pretty -c \"COMMAND\"\n");
}

int main(int argc, char **argv)
{
    FILE *input = NULL;
    int pretty = 0;

    if (argc < 2)
    {
        fprintf(stderr, "Bad usage\n");
        usage(stderr);
        return 1;
    }

    /* Optional flag */
    int argi = 1;
    if (strcmp(argv[argi], "--pretty") == 0
        || strcmp(argv[argi], "--pretty-print") == 0)
    {
        pretty = 1;
        argi++;
        if (argi >= argc)
        {
            fprintf(stderr, "Bad usage\n");
            usage(stderr);
            return 1;
        }
    }

    /* Read from file */
    if (argc - argi == 1)
    {
        input = fopen(argv[argi], "r");
        if (!input)
        {
            perror("fopen");
            return 1;
        }
    }
    /* Read string: -c "..." */
    else if (argc - argi == 2 && strcmp(argv[argi], "-c") == 0)
    {
        input = io_string_to_file(argv[argi + 1]);
        if (!input)
        {
            fprintf(stderr, "io_string_to_file failed\n");
            return 1;
        }
    }
    else
    {
        fprintf(stderr, "Bad usage\n");
        usage(stderr);
        return 1;
    }

    ast_t *ast = parse_input(input);
    if (!ast)
    {
        fprintf(stderr, "Parse error\n");
        fclose(input);
        return 1;
    }

    if (pretty)
        ast_pretty_print(ast, stdout);
    else
        ast_printer(ast, 0);

    ast_free(ast);
    fclose(input);
    return 0;
}