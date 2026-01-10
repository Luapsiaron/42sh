#include "parser_internal.h"

token_type_t peek(parser_t *p)
{
    if (!p || !p->current_token)
    {
        return TOKEN_EOF;
    }
    return p->current_token->type;
}

void pop(parser_t *p)
{
    if (!p)
    {
        return;
    }

    if (p->current_token)
    {
        token_free(p->current_token);
    }

    p->current_token = lexer_next(&p->lexer);
}

int is_semicolon_newline(token_type_t t)
{
    return t == TOKEN_SEMICOLON || t == TOKEN_NEWLINE;
}

void skip_semicolon_newline(parser_t *p)
{
    while (is_semicolon_newline(peek(p)))
    {
        pop(p);
    }
}

/* ==================================================================================
 */
/*
file f = null
char * string = null
parser_input(get_file(f, string))
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

static ast_t *parser_if(lexer_t lx)
{
    ast_t *new = ast_if_init(NULL, NULL, NULL);
    if (!new)
    {
        return NULL;
    }

    token_t *token = lexer_next(lx);
    if (!token || token->type != TOKEN_WORD)
    {
        ast_free(new);
        return NULL;
    }

    new->data.ast_if.condition = parser_create_argv(lx, token);
}

ast_t *parser(lexer_t lx)
{
    token_t *token = NULL;
    // a changer le while on peut pas verif si le token a pas foirer son calloc
    // aussi c quoi la struct tt en haut une ast_list? comme ça on peut alterner
    // echo bonjour;
    // if ...
    // then
    //    echo 1;
    //    echo 2
    // dans le même input
    token = lexer_next(lx);
    if (!token)
    {
        return NULL;
    }

    while (token->type != TOKEN_EOF)
    {
        if (token.type == TOKEN_IF)
        {
            return parser_if(lx);
        }

        if (token)
        {
            free(token);
        }
        token = lexer_next(lx);

        if (!token)
        {
            return NULL;
        }
    }
}
*/
