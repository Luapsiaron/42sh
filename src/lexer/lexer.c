#include "lexer.h"

#include <stdlib.h>
#include <string.h>

#include "lexer_internal.h"

void lexer_init(struct lexer *lx, FILE *input) // initializes the lexer
{
    lx->input = input;
    lx->current = fgetc(input);
    lx->condition = LEXER_NORMAL;
    lx->error = 0;
}

int lexer_error_occured(
    const struct lexer *lx) // checks if an error occurred during lexing
{
    return lx && lx->error;
}

/*
    Return next token from input corresponding to his type
*/
struct token *lexer_next(struct lexer *lx)
{
    if (lx->error)
    {
        return NULL;
    }

    struct token *token = NULL;

    skip_blanks(lx);

    if ((token = handle_comment(lx)) != NULL)
    {
        return token;
    }
    if ((token = handle_separator(lx)) != NULL)
    {
        lx->condition = LEXER_NORMAL;
        return token;
    }
    if ((token = handle_redirection(lx)) != NULL)
    {
        lx->condition = LEXER_FORCE_WORD;
        return token;
    }
    if ((token = handle_and_or(lx)) != NULL)
    {
        if (token->type == TOKEN_AND_IF)
        {
            lx->condition = LEXER_NORMAL;
        }
        else
        {
            lx->condition = LEXER_WORD_UNTIL;
        }
        return token;
    }
    if ((token = handle_pipe_or(lx)) != NULL)
    {
        lx->condition = LEXER_NORMAL;
        return token;
    }
    if ((token = handle_negation(lx)) != NULL)
    {
        lx->condition = LEXER_NORMAL;
        return token;
    }
    if (isdigit(lx->current))
    {
        token = lexer_ionumber(lx);
        lx->condition = LEXER_WORD_UNTIL;
        return token;
    }
    if (lx->current == EOF)
    {
        return token_new(TOKEN_EOF, NULL);
    }
    return lexer_is_word(lx);
}
