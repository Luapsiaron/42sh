#include "lexer.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/stack/stack.h"

static void lexer_next_char(lexer_t *lx)
{
    lx->current = fgetc(lx->input);
}

static void lexer_skip_comment(lexer_t *lx)
{
    while (lx->current != EOF && lx->current != '\n')
        lexer_next_char(lx);
}

static int is_redir_char(int c)
{
    return c == '>' || c == '<';
}

static token_t *lexer_ionumber(lexer_t *lx)
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

static int is_assignment_word(const char *s)
{
    if (!s || !s[0])
    {
        return 0;
    }

    if (!isalpha(s[0]) && s[0] != '_')
    {
        return 0;
    }

    size_t i = 1;
    while (s[i] && (isalnum(s[i]) || isalpha(s[i]) || s[i] == '_'))
    {
        i++;
    }
    return s[i] == '=';
}

static token_t *lexer_is_word(lexer_t *lx)
{
    char buffer[512];
    size_t i = 0;

    while (lx->current != EOF && lx->current != ';' && lx->current != '|'
           && lx->current != '>' && lx->current != '<' && lx->current != '\n'
           && !isspace(lx->current))
    {
        buffer[i++] = lx->current;
        lexer_next_char(lx);
    }
    buffer[i] = '\0';

    if (is_assignment_word(buffer))
    {
        lx->condition = LEXER_WORD_UNTIL;
        return token_new(TOKEN_ASSIGNMENT_WORD, buffer);
    }

    if (lx->condition == LEXER_FORCE_WORD)
    {
        lx->condition = LEXER_WORD_UNTIL;
        return token_new(TOKEN_WORD, buffer);
    }

    if (lx->condition == LEXER_NORMAL)
    {
        token_type_t type;
        if (token_is_reserved_word(buffer, &type))
            return token_new(type, NULL);
    }

    lx->condition = LEXER_WORD_UNTIL;
    return token_new(TOKEN_WORD, buffer);
}

static void skip_blanks(lexer_t *lx)
{
    while (lx->current != EOF && isspace(lx->current) && lx->current != '\n')
    {
        lexer_next_char(lx);
    }
}

static token_t *handle_comment(lexer_t *lx)
{
    if (lx->current != '#')
    {
        return NULL;
    }
    lexer_skip_comment(lx);
    return token_new(TOKEN_NEWLINE, NULL);
}

static token_t *handle_separator(lexer_t *lx)
{
    if (lx->current == ';')
    {
        lexer_next_char(lx);
        lx->condition = LEXER_NORMAL;
        return token_new(TOKEN_SEMICOLON, NULL);
    }
    if (lx->current == '\n')
    {
        lexer_next_char(lx);
        lx->condition = LEXER_NORMAL;
        return token_new(TOKEN_NEWLINE, NULL);
    }
    return NULL;
}

static token_t *handle_redirection(lexer_t *lx)
{
    if (lx->current == '<')
    {
        lexer_next_char(lx);
        lx->condition = LEXER_FORCE_WORD;
        return token_new(TOKEN_LESS, NULL);
    }
    if (lx->current != '>')
    {
        return NULL;
    }
    lexer_next_char(lx);
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

static token_t *handle_and_or(lexer_t *lx)
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

static token_t *handle_pipe_or(lexer_t *lx)
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

static token_t *handle_negation(lexer_t *lx)
{
    if (lx->current != '!')
    {
        return NULL;
    }
    lexer_next_char(lx);
    lx->condition = LEXER_NORMAL;
    return token_new(TOKEN_NEGATION, NULL);
}

void lexer_init(lexer_t *lx, FILE *input)
{
    lx->input = input;
    lx->current = fgetc(input);
    lx->condition = LEXER_NORMAL;
}

token_t *lexer_next(lexer_t *lx)
{
    token_t *token = NULL;

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
        if(token->type == TOKEN_AND_IF)
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
