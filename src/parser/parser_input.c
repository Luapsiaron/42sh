#include "parser.h"
#include "parser_internal.h"

ast_t *parse_input(FILE *f)
{
    parser_t p;
    p.current_token = NULL;

    lexer_init(&p.lexer, f);
    p.current_token = lexer_next(&p.lexer);
    if (!p.current_token)
    {
        return NULL;
    }

    skip_semicolon_newline(&p);
    if (peek(&p) == TOKEN_EOF)
    {
        token_free(p.current_token);
        p.current_token = NULL;
        return NULL;
    }

    ast_t *root = parse_list(&p);
    if (!root)
    {
        if (p.current_token)
        {
            token_free(p.current_token);
        }
        return NULL;
    }

    skip_semicolon_newline(&p);
    if (peek(&p) != TOKEN_EOF)
    {
        ast_free(root);
        if (p.current_token)
        {
            token_free(p.current_token);
        }
        return NULL;
    }

    token_free(p.current_token);
    p.current_token = NULL;
    return root;
}