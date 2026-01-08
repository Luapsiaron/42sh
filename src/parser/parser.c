#include "parser.h"

#include "../lexer/lexer.h"

// a bouger dans un ficheir dédié plus tard
static free_argv(char **argv)
{
    if (argv)
    {
        for (int i = 0; argv[i]; i++)
        {
            free(argv[i]);
        }
        free(argv);
    }
}

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
            f = io_string_to_file();
        }
    }
    return f;
}

ast_t parser_init(FILE *f, char *string)
{
    f = get_file(f, string);

    lexer_t lx = NULL;
    lexer_init(lx, f);

    return parser(lx);
}

static char **parser_create_argv(lexer_t lx, token_t token)
{
    int size = 2;
    char **argv = calloc(sizeof(char *) * size);
    if (!argv)
    {
        return NULL;
    }

    argv[0] = token.lexeme;
    argv[1] = NULL;
    free(token);

    int is_word = 1;
    while (is_word)
    {
        token = lexer_next(lx);
        if (!token)
        {
            free_argv(argv);
        }
        // prendre tt les mots prcq si on verif que word "echo if" marchera pas
        // car if -> TOEKN_IF
        if (token.type != TOKEN_SEMICOLON && token.type != TOKEN_NEWLINE
            && token.type != TOKEN_EOF)
        {
            size++;
            argv = realloc(argv, sizeof(char *) * size);
            if (!argv)
            {
                return NULL;
            }
            // size est minimum de taille 3 ici
            argv[size - 2] = token.lexeme;
            argv[size - 1] = NULL;
        }
        else
        {
            is_word = 0;
        }
    }
    return argv;
}

static ast_t parser_if(lexer_t lx)
{
    ast_t new = ast_if_init(NULL, NULL, NULL);
    if (!new)
    {
        return NULL;
    }

    token_t token = lexer_next(lx);
    if (!token || token.type != TOKEN_WORD)
    {
        ast_free(new);
        return NULL;
    }

    new->data.ast_if.condition = parser_create_argv(lx, token);
}

ast_t parser(lexer_t lx)
{
    token_t token = NULL;
    // a changer le while on peut pas verif si le token a pas foirer son calloc
    // aussi c quoi la struct tt en haut une ast_list? comme ça on peut alterner
    // echo bonjour;
    // if ...
    // dans le même input
    while ((token = lexer_next(lx)).type != TOKEN_EOF)
    {
        if (token.type == TOKEN_IF)
        {
            return parser_if(lx);
        }

        if (token)
        {
            free(token);
        }
    }
}
