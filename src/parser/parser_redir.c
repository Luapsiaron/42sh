#include <stdlib.h>

#include "parser_internal.h"

int is_redirection_token(enum token_type type)
{
    return type == TOKEN_LESS || type == TOKEN_GREAT || type == TOKEN_DGREAT
        || type == TOKEN_CLOBBER || type == TOKEN_LESSAND
        || type == TOKEN_GREATAND || type == TOKEN_LESSGREAT;
}

static enum redir_type token_to_redir_type(enum token_type type)
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
    if (type == TOKEN_CLOBBER)
    {
        return REDIR_CLOBBER;
    }
    if (type == TOKEN_GREATAND)
    {
        return REDIR_DUP_OUT;
    }
    if (type == TOKEN_LESSAND)
    {
        return REDIR_DUP_IN;
    }
    if (type == TOKEN_LESSGREAT)
    {
        return REDIR_INOUT;
    }
    return REDIR_OUT;
}

/*
    Parse a redirection
    Grammar: redirection = [ IO_NUMBER ] redir_op WORD

    1. Optionally parses an IO_NUMBER
    2. Parses a redirection operator (e.g., <, >, >>, >|)
    3. Parses a WORD token representing the file or descriptor

    Examples:
    - >out => io_number = 1, type = REDIR_OUT, word = "out"
    - 2>err => io_number = 2, type = REDIR_OUT, word = "err"
    - <in => io_number = 0, type = REDIR_IN, word = "in"
    - 3>>log => io_number = 3, type = REDIR_APPEND, word = "log"

    If no IO_NUMBER is provided:
    - For input redirections (<), defaults to 0 (stdin)
    - For output redirections (>, >>, >|), defaults to 1 (stdout)
*/
struct ast *parse_redirection(struct parser *p)
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

    enum redir_type type = token_to_redir_type(peek(p));
    pop(p);

    if (peek(p) != TOKEN_WORD)
    {
        return NULL;
    }

    if (io_number < 0)
    {
        if (type == REDIR_IN || type == REDIR_DUP_IN || type == REDIR_INOUT)
        {
            io_number = 0;
        }
        else
        {
            io_number = 1;
        }
    }

    struct ast *redir =
        ast_redir_init(io_number, type, p->current_token->lexeme, NULL);
    if (!redir)
    {
        return NULL;
    }

    pop(p);
    return redir;
}
