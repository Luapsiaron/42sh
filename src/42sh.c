#define _XOPEN_SOURCE
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "execution/execution.h"
#include "io/io.h"
#include "parser/parser.h"
#include "utils/parser/pretty_printer.h"
#include "expansion/hashmap.h"

static void usage(FILE *out)
{
    fprintf(out,
            "usage: 42sh [OPTIONS] [SCRIPT] [ARGUMENTS ...]\n"
            "Options:\n"
            "  -c \"SCRIPT\"   read commands from SCRIPT string\n"
            "  -e FILE.sh      read commands from FILE.sh\n"
            "  <  FILE.sh      read commands from standard input\n"
            " --pretty-print   print the parsed AST in a pretty format\n"
            " PRETTY_PRINT=1   same as --pretty-print\n");
}

static void error_usage(const char *msg)
{
    if (msg)
        fprintf(stderr, "42sh: %s\n", msg);
    usage(stderr);
    exit(2);
}

static int run_stream(FILE *input, int pretty_print, struct hash_map *hm)
{
    int status = 0;

    while (1)
    {
        ast_t *tree = parse_input(input);
        // ast_printer(tree, 0);
        if (!tree)
            break;
        if (pretty_print)
        {
            ast_pretty_print(tree, stdout);
            ast_free(tree);
            return 0;
        }
        status = exec_ast(tree, hm);
        ast_free(tree);
    }
    return status;
}

int main(int argc, char **argv, char **envp) // envp -> collect system variables
{
    int opt;
    char *command = NULL;
    int pretty_print = 0;

    struct hash_map *hm = hash_map_init(100); // 100 = magic value

    for(int i = 0; envp[i]; i++)
    {
        char *eq = strchr(envp[i], '=');
        if(eq)
        {
            *eq = '\0';
            hash_map_insert(hm, envp[i], eq + 1, NULL);
            *eq = '=';
        }
    }

    if (getenv("PRETTY_PRINT") != NULL)
    {
        pretty_print = 1;
    }

    for (int i = 1; i < argc; ++i)
    {
        if (strcmp(argv[i], "--pretty-print") == 0)
        {
            pretty_print = 1;
            for (int j = i; j < argc - 1; ++j)
            {
                argv[j] = argv[j + 1];
            }
            --argc;
            --i;
        }
    }

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
            hash_map_free(hm);
            exit(2);
        }
        must_close = true;
    }
    else
    {
        input = stdin;
        must_close = false;
    }
    int status = run_stream(input, pretty_print, hm);
    if (must_close && input)
        fclose(input);

    hash_map_free(hm);
    return status;
}
