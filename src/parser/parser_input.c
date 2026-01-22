#include "parser.h"
#include "parser_internal.h"

static int parse_error = 0;

int parse_error_occurred(void)
{
    return parse_error;
}

void parse_set_error(void)
{
    parse_error = 1;
}

/*
    Parse the entire input from the given file
    Returns the root AST node or NULL on error
*/
struct ast *parse_input(FILE *f)
{
    parse_error = 0;

    struct parser p;
    p.current_token = NULL;
    p.next_token = NULL;

    lexer_init(&p.lexer, f);
    p.current_token = lexer_next(&p.lexer);
    if (!p.current_token)
    {
        if(lexer_error_occured(&p.lexer))
        {
            parse_error = 1;
        }
        return NULL;
    }

    skip_newlines(&p);
    if(peek(&p) == TOKEN_EOF)
    {
        token_free(p.current_token);
        token_free(p.next_token);
        return NULL;
    }
    if (peek(&p) == TOKEN_SEMICOLON)
    {
        goto error;
    }

    struct ast *root = parse_list(&p);
    if (!root)
    {
        goto error;
    }

    skip_semicolon_newline(&p);
    if (peek(&p) != TOKEN_EOF)
    {
        ast_free(root);
        goto error;
    }

    token_free(p.current_token);
    token_free(p.next_token);
    return root;

error:
    parse_error = 1;
    token_free(p.current_token);
    token_free(p.next_token);
    return NULL;
}
