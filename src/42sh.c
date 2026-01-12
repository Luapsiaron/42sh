#define _XOPEN_SOURCE
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ast/ast.h"
#include "execution/execution.h"
#include "io/io.h"
#include "parser/parser.h"

static void usage(FILE *out)
{
    fprintf(out,
            "usage: 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]\n"
            "Options:\n"
            "  -c \"SCRIPT\"   read commands from SCRIPT string\n"
            "  -e FILE.sh      read commands from FILE.sh\n"
            "  <  FILE.sh      read commands from standard input\n");
}

static void error_usage(const char *msg)
{
    if (msg)
        fprintf(stderr, "42sh: %s\n", msg);
    usage(stderr);
    exit(1);
}

static int run_stream(FILE *input)
{
    int status = 0;

    while (1)
    {
        ast_t *tree = parse_input(input);
        // ast_printer(tree, 0);
        if (!tree)
            break;
        status = exec_ast(tree);
        ast_free(tree);
    }
    return status;
}

int main(int argc, char **argv)
{
    int opt;
    char *command = NULL;
    while ((opt = getopt(argc, argv, "c:")) != -1)
    {
        switch (opt)
        {
        case 'c':
            command = optarg;
            break;
        case '?':
            if (optopt == 'c')
                error_usage("option -c requires an argument");
            error_usage("invalid option");
            break;
        default:
            error_usage("invalid option");
        }
    }
    int remaining = argc - optind;
    FILE *input = NULL;
    bool must_close = false;
    if (command != NULL)
    {
        if (remaining > 0)
            error_usage("cannot use -c with a script file operand");
        input = io_string_to_file(command);
        if (!input)
            error_usage("failed to open script string");
        must_close = true;
    }
    else if (remaining >= 1)
    {
        const char *path = argv[optind];
        input = fopen(path, "r");
        if (!input)
        {
            fprintf(stderr, "42sh: cannot open '%s': %s\n", path,
                    strerror(errno));
            exit(1);
        }
        must_close = true;
    }
    else
    {
        input = stdin;
        must_close = false;
    }
    int status = run_stream(input);
    if (must_close && input)
        fclose(input);
    return status;
}
