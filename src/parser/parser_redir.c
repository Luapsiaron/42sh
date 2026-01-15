#include <stdlib.h>

#include "parser_internal.h"

int is_redirection_token(token_type_t type)
{
    return type == TOKEN_LESS || type == TOKEN_GREAT || type == TOKEN_DGREAT
        || type == TOKEN_CLOBBER;
}

static redir_type_t token_to_redir_type(token_type_t type)
{
    if (type == TOKEN_LESS)
    {
        return REDIR_IN;
    }
    if (type == TOKEN_GREAT)
    {
        return REDIR_OUT;
    }
    if (type == TOKEN_DGREAT)
    {
        return REDIR_APPEND;
    }

    return REDIR_CLOBBER;
}

ast_t *parse_redirection(parser_t *p)
{
    int io_number = -1;

    if (peek(p) == TOKEN_IONUMBER)
    {
        io_number = atoi(p->current_token->lexeme);
        pop(p);
    }

    if (!is_redirection_token(peek(p)))
    {
        return NULL;
    }

    redir_type_t type = token_to_redir_type(peek(p));
    pop(p);

    if (peek(p) != TOKEN_WORD)
    {
        return NULL;
    }

    if (io_number < 0)
    {
        io_number = (type == REDIR_IN) ? 0 : 1;
    }

    ast_t *redir =
        ast_redir_init(io_number, type, p->current_token->lexeme, NULL);
    if (!redir)
    {
        return NULL;
    }

    pop(p);
    return redir;
}