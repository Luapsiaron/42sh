#include "lexer_internal.h"

/* ----------------- Comment -----------------*/

static void lexer_skip_comment(struct lexer *lx)
{
    while (lx->current != EOF && lx->current != '\n')
        lexer_next_char(lx);
}

struct token *handle_comment(struct lexer *lx)
{
    if (lx->current != '#')
    {
        return NULL;
    }
    lexer_skip_comment(lx);
    return token_new(TOKEN_NEWLINE, NULL);
}

/* ----------------- Separator -----------------*/

struct token *handle_separator(struct lexer *lx)
{
    switch (lx->current)
    {
    case ';':
        lexer_next_char(lx);
        lx->condition = LEXER_NORMAL;
        return token_new(TOKEN_SEMICOLON, NULL);
    case '\n':
        lexer_next_char(lx);
        lx->condition = LEXER_NORMAL;
        return token_new(TOKEN_NEWLINE, NULL);
    case '{':
        lexer_next_char(lx);
        lx->condition = LEXER_NORMAL;
        return token_new(TOKEN_LBRACE, NULL);
    case '}':
        lexer_next_char(lx);
        lx->condition = LEXER_NORMAL;
        return token_new(TOKEN_RBRACE, NULL);
    case '(':
        lexer_next_char(lx);
        lx->condition = LEXER_NORMAL;
        return token_new(TOKEN_LPAREN, NULL);
    case ')':
        lexer_next_char(lx);
        lx->condition = LEXER_NORMAL;
        return token_new(TOKEN_RPAREN, NULL);
    default:
        return NULL;
    }
}
/* ----------------- IO Number -----------------*/

static int is_redir_char(int c)
{
    return c == '>' || c == '<';
}

struct token *lexer_ionumber(struct lexer *lx)
{
    char buffer[32];
    size_t i = 0;

    while (isdigit(lx->current) && i < sizeof(buffer) - 1)
    {
        buffer[i++] = lx->current;
        lexer_next_char(lx);
    }
    buffer[i] = '\0';

    if (is_redir_char(lx->current))
    {
        return token_new(TOKEN_IONUMBER, buffer);
    }
    return token_new(TOKEN_WORD, buffer);
}

/* ----------------- Redirection -----------------*/

struct token *handle_redirection(struct lexer *lx)
{
    if (lx->current == '<')
    {
        lexer_next_char(lx);
        if (lx->current == '&')
        {
            lexer_next_char(lx);
            lx->condition = LEXER_FORCE_WORD;
            return token_new(TOKEN_LESSAND, NULL);
        }

        if (lx->current == '>')
        {
            lexer_next_char(lx);
            lx->condition = LEXER_FORCE_WORD;
            return token_new(TOKEN_LESSGREAT, NULL);
        }
        lx->condition = LEXER_FORCE_WORD;
        return token_new(TOKEN_LESS, NULL);
    }
    if (lx->current != '>')
    {
        return NULL;
    }
    lexer_next_char(lx);
    if (lx->current == '&')
    {
        lexer_next_char(lx);
        lx->condition = LEXER_FORCE_WORD;
        return token_new(TOKEN_GREATAND, NULL);
    }
    lx->condition = LEXER_FORCE_WORD;
    if (lx->current == '>')
    {
        lexer_next_char(lx);
        return token_new(TOKEN_DGREAT, NULL);
    }
    if (lx->current == '|')
    {
        lexer_next_char(lx);
        return token_new(TOKEN_CLOBBER, NULL);
    }
    return token_new(TOKEN_GREAT, NULL);
}

/* ----------------- && / || / | -----------------*/

struct token *handle_and_or(struct lexer *lx)
{
    if (lx->current != '&')
    {
        return NULL;
    }
    lexer_next_char(lx);
    if (lx->current == '&')
    {
        lexer_next_char(lx);
        lx->condition = LEXER_NORMAL;
        return token_new(TOKEN_AND_IF, NULL);
    }
    lx->condition = LEXER_WORD_UNTIL;
    return token_new(TOKEN_WORD, "&");
}

struct token *handle_pipe_or(struct lexer *lx)
{
    if (lx->current != '|')
    {
        return NULL;
    }
    lexer_next_char(lx);
    if (lx->current == '|')
    {
        lexer_next_char(lx);
        lx->condition = LEXER_NORMAL;
        return token_new(TOKEN_OR_IF, NULL);
    }
    lx->condition = LEXER_NORMAL;
    return token_new(TOKEN_PIPE, NULL);
}

/* ----------------- ! -----------------*/

struct token *handle_negation(struct lexer *lx)
{
    if (lx->current != '!')
    {
        return NULL;
    }
    lexer_next_char(lx);
    lx->condition = LEXER_NORMAL;
    return token_new(TOKEN_NEGATION, NULL);
}
