#include "lexer_internal.h"

/* ----------------- Helpers -----------------*/

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

static int append_buffer(char *buffer, size_t *index, size_t capacity, int c)
{
    if (*index >= capacity - 1)
    {
        return 0;
    }
    buffer[(*index)++] = (char)c;
    return 1;
}

static int lexer_single_quotes(struct lexer *lx, char *buffer, size_t *index,
                               size_t capacity)
{
    if (!append_buffer(buffer, index, capacity, '\''))
    {
        return 0;
    }
    lexer_next_char(lx);

    while (lx->current != EOF && lx->current != '\'')
    {
        if (!append_buffer(buffer, index, capacity, lx->current))
        {
            return 0;
        }
        lexer_next_char(lx);
    }

    if (lx->current != '\'')
    {
        return 0;
    }

    if (!append_buffer(buffer, index, capacity, '\''))
    {
        return 0;
    }
    lexer_next_char(lx);
    return 1;
}

static int lexer_double_quotes(struct lexer *lx, char *buffer, size_t *index,
                               size_t capacity)
{
    if (!append_buffer(buffer, index, capacity, '"'))
    {
        return 0;
    }
    lexer_next_char(lx);

    while (lx->current != EOF && lx->current != '"')
    {
        if (lx->current == '\\')
        {
            lexer_next_char(lx);
            if (lx->current == EOF)
            {
                return 0;
            }

            if (lx->current == '\n')
            {
                lexer_next_char(lx);
                continue;
            }

            if (!append_buffer(buffer, index, capacity, '\\'))
            {
                return 0;
            }
            if (!append_buffer(buffer, index, capacity, lx->current))
            {
                return 0;
            }
            lexer_next_char(lx);
            continue;
        }
        if (!append_buffer(buffer, index, capacity, lx->current))
        {
            return 0;
        }
        lexer_next_char(lx);
    }

    if (lx->current != '"')
    {
        return 0;
    }
    if (!append_buffer(buffer, index, capacity, '"'))
    {
        return 0;
    }

    lexer_next_char(lx);
    return 1;
}

/* ----------------- Word -----------------*/

struct token *lexer_is_word(struct lexer *lx)
{
    char buffer[512];
    size_t i = 0;
    int quote = 0;

    while (lx->current != EOF && lx->current != ';' && lx->current != '|'
           && lx->current != '>' && lx->current != '<' && lx->current != '\n'
           && lx->current != '{' && lx->current != '}' && lx->current != '(' 
           && lx->current != ')' && !isspace(lx->current))
    {
        if (lx->current == '\'')
        {
            quote = 1;
            if (!lexer_single_quotes(lx, buffer, &i, sizeof(buffer)))
            {
                lx->error = 1;
                return NULL;
            }
            continue;
        }
        if (lx->current == '"')
        {
            quote = 1;
            if (!lexer_double_quotes(lx, buffer, &i, sizeof(buffer)))
            {
                lx->error = 1;
                return NULL;
            }
            continue;
        }
        if (!append_buffer(buffer, &i, sizeof(buffer), lx->current))
        {
            lx->error = 1;
            return NULL;
        }
        lexer_next_char(lx);
    }
    buffer[i] = '\0';

    if (is_assignment_word(buffer))
    {
        return token_new(TOKEN_ASSIGNMENT_WORD, buffer);
    }

    if (lx->condition == LEXER_FORCE_WORD)
    {
        lx->condition = LEXER_WORD_UNTIL;
        return token_new(TOKEN_WORD, buffer);
    }

    enum token_type type;
    if (!quote && lx->condition == LEXER_NORMAL
        && token_is_reserved_word(buffer, &type))
    {
        return token_new(type, NULL);
    }

    lx->condition = LEXER_WORD_UNTIL;
    return token_new(TOKEN_WORD, buffer);
}