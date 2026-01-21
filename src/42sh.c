#define _XOPEN_SOURCE
#include <errno.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "execution/execution.h"
#include "expansion/hashmap.h"
#include "io/io.h"
#include "parser/parser.h"
#include "utils/parser/pretty_printer.h"

struct input_sel_ctx
{
    int argc;
    char **argv;
    char *command;
    struct hash_map *hm;
};

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
        struct ast *tree = parse_input(input);
        // ast_printer(tree, 0);
        if (!tree)
        {
            if (parse_error_occurred())
            {
                fprintf(stderr, "42sh: syntax error\n");
                return 2;
            }
            break;
        }
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

static void init_env_hashmap(struct hash_map *hm, char **envp)
{
    for (int i = 0; envp[i]; i++)
    {
        char *eq = strchr(envp[i], '=');
        if (!eq)
            continue;
        *eq = '\0';
        hash_map_insert(hm, envp[i], eq + 1, NULL);
        *eq = '=';
    }
}

static FILE *select_input_stream(struct input_sel_ctx c, bool *must_close)
{
    int remaining = c.argc - optind;
    *must_close = false;

    if (c.command != NULL)
    {
        // With -c, the remaining operands are positional parameters ($1..$n).
        FILE *in = io_string_to_file(c.command);
        if (!in)
            error_usage("failed to open script string");
        *must_close = true;
        return in;
    }

    if (remaining >= 1)
    {
        const char *path = c.argv[optind];
        FILE *in = fopen(path, "r");
        if (!in)
        {
            fprintf(stderr, "42sh: cannot open '%s': %s\n", path,
                    strerror(errno));
            hash_map_free(c.hm);
            exit(2);
        }
        *must_close = true;
        return in;
    }
    return stdin;
}


static void init_positional_params(struct hash_map *hm, int argc, char **argv,
                                   int start_index)
{
    if (!hm)
        return;
    if (argv && argv[0])
        hash_map_insert(hm, "0", argv[0], NULL);

    if (start_index < 0)
        start_index = 0;
    if (!argv || start_index >= argc)
        return;

    int pos = 1;
    for (int i = start_index; i < argc; i++, pos++)
    {
        char key[16];
        snprintf(key, sizeof(key), "%d", pos);
        hash_map_insert(hm, key, argv[i], NULL);
    }
}

int main(int argc, char **argv, char **envp)
{
    int opt, pretty_print = 0;
    char *command = NULL;
    struct hash_map *hm = hash_map_init(100);

    init_env_hashmap(hm, envp);
    if (getenv("PRETTY_PRINT") != NULL)
        pretty_print = 1;

    for (int i = 1; i < argc; ++i)
        if (strcmp(argv[i], "--pretty-print") == 0)
        {
            pretty_print = 1;
            for (int j = i; j < argc - 1; ++j)
                argv[j] = argv[j + 1];
            --argc;
            --i;
        }

    while ((opt = getopt(argc, argv, "c:")) != -1)
        if (opt == 'c')
            command = optarg;
        else if (opt == '?' && optopt == 'c')
            error_usage("option -c requires an argument");
        else
            error_usage("invalid option");
    if (command != NULL)
    {
        init_positional_params(hm, argc, argv, optind);
    }
    else if (argc - optind >= 1)
    {
        init_positional_params(hm, argc, argv, optind + 1);
    }

    bool must_close = false;
    struct input_sel_ctx c = { argc, argv, command, hm };
    FILE *input = select_input_stream(c, &must_close);

    int status = run_stream(input, pretty_print, hm);
    if (must_close && input)
        fclose(input);
    hash_map_free(hm);
    return status;
}

