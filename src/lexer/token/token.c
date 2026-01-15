#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/str/str.h"

token_t *token_new(token_type_t type, const char *value)
{
    token_t *t = calloc(1, sizeof(*t));
    if (!t)
    {
        return NULL;
    }

    t->type = type;
    if (value)
    {
        t->lexeme = xstrdup(value);
        if (!t->lexeme)
        {
            free(t);
            return NULL;
        }
    }
    return t;
}

void token_free(token_t *t)
{
    if (!t)
    {
        return;
    }
    free(t->lexeme);
    free(t);
}

int token_is_reserved_word(const char *s, token_type_t *out)
{
    if (!s || !out)
    {
        return 0;
    }
    if (!strcmp(s, "if"))
    {
        *out = TOKEN_IF;
        return 1;
    }

    if (!strcmp(s, "then"))
    {
        *out = TOKEN_THEN;
        return 1;
    }
    if (!strcmp(s, "else"))
    {
        *out = TOKEN_ELSE;
        return 1;
    }
    if (!strcmp(s, "elif"))
    {
        *out = TOKEN_ELIF;
        return 1;
    }
    if (!strcmp(s, "fi"))
    {
        *out = TOKEN_FI;
        return 1;
    }
    if (!strcmp(s, "for"))
    {
        *out = TOKEN_FOR;
        return 1;
    }
    if (!strcmp(s, "while"))
    {
        *out = TOKEN_WHILE;
        return 1;
    }
    if (!strcmp(s, "until"))
    {
        *out = TOKEN_UNTIL;
        return 1;
    }
    if (!strcmp(s, "in"))
    {
        *out = TOKEN_IN;
        return 1;
    }
    if (!strcmp(s, "do"))
    {
        *out = TOKEN_DO;
        return 1;
    }
    if (!strcmp(s, "done"))
    {
        *out = TOKEN_DONE;
        return 1;
    }
    return 0;
}

const char *token_type_name(token_type_t t)
{
    switch (t)
    {
    case TOKEN_IF:
        return "IF";
    case TOKEN_THEN:
        return "THEN";
    case TOKEN_ELSE:
        return "ELSE";
    case TOKEN_ELIF:
        return "ELIF";
    case TOKEN_FI:
        return "FI";
    case TOKEN_SEMICOLON:
        return "SEMICOLON";
    case TOKEN_NEWLINE:
        return "NEWLINE";
    case TOKEN_WORD:
        return "WORD";
    case TOKEN_PIPE:
        return "PIPE";
    case TOKEN_LESS:
        return "<";
    case TOKEN_GREAT:
        return ">";
    case TOKEN_DGREAT:
        return ">>";
    case TOKEN_CLOBBER:
        return ">|";
    case TOKEN_NEGATION:
        return "NEGATION";
    case TOKEN_AND_IF:
        return "AND_IF";
    case TOKEN_OR_IF:
        return "OR_IF";
    case TOKEN_WHILE:
        return "WHILE";
    case TOKEN_FOR:
        return "FOR";
    case TOKEN_IN:
        return "IN";
    case TOKEN_DO:
        return "DO";
    case TOKEN_DONE:
        return "DONE";
    case TOKEN_UNTIL:
        return "UNTIL";
    case TOKEN_EOF:
        return "EOF";
    default:
        return "UNKNOWN";
    }
}

void token_printer(const token_t *t)
{
    if (!t)
    {
        printf("(null token)\n");
        return;
    }
    printf("%s", token_type_name(t->type));
    if (t->lexeme)
    {
        printf("(\"%s\")", t->lexeme);
    }
    printf("\n");
}
