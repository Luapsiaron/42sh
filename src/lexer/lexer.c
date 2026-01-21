#include "lexer.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/stack/stack.h"

static void lexer_next_char(struct lexer *lx)
{
    lx->current = fgetc(lx->input);
}

static void lexer_skip_comment(struct lexer *lx)
{
    while (lx->current != EOF && lx->current != '\n')
        lexer_next_char(lx);
}

static int is_redir_char(int c)
{
    return c == '>' || c == '<';
}

/*
    Lex an IO_NUMBER token or a WORD token starting with digits
    An IO_NUMBER is a sequence of digits followed by a redirection character
    If the next character after the digits is not a redirection character,
    it is treated as a WORD token
    Examples:
    - "2>" -> TOKEN_IONUMBER with lexeme "2"
*/
static struct token *lexer_ionumber(struct lexer *lx)
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

/*
    Check if the given string is a valid assignment word
    An assignment word matches the pattern: NAME=VALUE
    where NAME starts with a letter or underscore, followed by letters,
    digits, or underscores, and is followed by an '=' character
*/
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

/*
    Lex a single-quoted string, including the surrounding quotes
    Returns 1 on success, 0 on failure (e.g., unmatched quote or buffer overflow)
*/
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

/*
    Lex a double-quoted string, including the surrounding quotes
    Handles escape sequences within the double quotes
    Returns 1 on success, 0 on failure (e.g., unmatched quote or buffer overflow)
*/
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

/*
    Lex a word token
    A word can contain letters, digits, underscores, and quoted strings
    It ends at whitespace, separators, or operators
*/
static struct token *lexer_is_word(struct lexer *lx)
{
    char buffer[512];
    size_t i = 0;
    int quote = 0;

    while (lx->current != EOF && lx->current != ';' && lx->current != '|'
           && lx->current != '>' && lx->current != '<' && lx->current != '\n'
           && lx->current != '{' && lx->current != '}' && !isspace(lx->current))
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

static void skip_blanks(struct lexer *lx)
{
    while (lx->current != EOF && isspace(lx->current) && lx->current != '\n')
    {
        lexer_next_char(lx);
    }
}

static struct token *handle_comment(struct lexer *lx)
{
    if (lx->current != '#')
    {
        return NULL;
    }
    lexer_skip_comment(lx);
    return token_new(TOKEN_NEWLINE, NULL);
}

static struct token *handle_separator(struct lexer *lx)
{
    if(lx->current != ';' && lx->current != '\n' && lx->current != '{' && lx->current != '}')
    {
        return NULL;
    }
    
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
    if (lx->current == '{')
    {
        lexer_next_char(lx);
        lx->condition = LEXER_NORMAL;
        return token_new(TOKEN_LBRACE, NULL);
    }
    if( lx->current == '}')
    {
        lexer_next_char(lx);
        lx->condition = LEXER_NORMAL;
        return token_new(TOKEN_RBRACE, NULL);
    }
    return NULL;
}

static struct token *handle_redirection(struct lexer *lx)
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

        if(lx->current == '>')
        {
            lexer_next_char(lx);
            lx->condition = LEXER_FORCE_WORD;
            return token_new(TOKEN_LESSGREAT, NULL);
        }
        lx->condition = LEXER_FORCE_WORD;
        return token_new(TOKEN_LESS, NULL);
    }
    if(lx->current != '>')
    {
        return NULL;
    }
    lexer_next_char(lx);
    if(lx->current == '&')
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

static struct token *handle_and_or(struct lexer *lx)
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

static struct token *handle_pipe_or(struct lexer *lx)
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

static struct token *handle_negation(struct lexer *lx)
{
    if (lx->current != '!')
    {
        return NULL;
    }
    lexer_next_char(lx);
    lx->condition = LEXER_NORMAL;
    return token_new(TOKEN_NEGATION, NULL);
}

void lexer_init(struct lexer *lx, FILE *input)
{
    lx->input = input;
    lx->current = fgetc(input);
    lx->condition = LEXER_NORMAL;
    lx->error = 0;
}

int lexer_error_occured(const struct lexer *lx)
{
    return lx && lx->error;
}

/*
    Return next token from input corresponding to his type
*/
struct token *lexer_next(struct lexer *lx)
{
    if(lx->error)
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
