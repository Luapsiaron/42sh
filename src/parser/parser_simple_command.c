#include <stdlib.h>

#include "../lexer/token/token.h"
#include "../utils/str/str.h"
#include "parser_internal.h"

ast_t *parse_simple_command(parser_t *p)
{
    skip_semicolon_newline(p);
    if (peek(p) != TOKEN_WORD)
    {
        return NULL;
    }

    char **argv = calloc(16, sizeof(char *));
    if (!argv)
    {
        return NULL;
    }

    int i = 0;
    // echo if then fi else elif
    while (!is_semicolon_newline(peek(p)) && peek(p) != TOKEN_EOF)
    {
        const char *token = NULL;

        if (peek(p) == TOKEN_WORD)
        {
            token = p->current_token->lexeme;
        }
        else
        {
            token = token_type_name(peek(p));
        }

        if (!token)
        {
            break;
        }

        argv[i] = xstrdup(token);

        if (!argv[i])
        {
            free_argv(argv);
            return NULL;
        }

        i++;
        pop(p);
    }
    argv[i] = NULL;

    return ast_cmd_init(argv);
}
