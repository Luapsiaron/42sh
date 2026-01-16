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

static const char *token_type_names[] = {
    [TOKEN_IF] = "IF",
    [TOKEN_THEN] = "THEN",
    [TOKEN_ELSE] = "ELSE",
    [TOKEN_ELIF] = "ELIF",
    [TOKEN_FI] = "FI",

    [TOKEN_FOR] = "FOR",
    [TOKEN_WHILE] = "WHILE",
    [TOKEN_UNTIL] = "UNTIL",
    [TOKEN_IN] = "IN",
    [TOKEN_DO] = "DO",
    [TOKEN_DONE] = "DONE",

    [TOKEN_SEMICOLON] = "SEMICOLON",
    [TOKEN_NEWLINE] = "NEWLINE",

    [TOKEN_WORD] = "WORD",
    [TOKEN_ASSIGNMENT_WORD] = "ASSIGNMENT_WORD",

    [TOKEN_PIPE] = "PIPE",
    [TOKEN_LESS] = "<",
    [TOKEN_GREAT] = ">",
    [TOKEN_DGREAT] = ">>",
    [TOKEN_CLOBBER] = ">|",

    [TOKEN_NEGATION] = "NEGATION",
    [TOKEN_AND_IF] = "AND_IF",
    [TOKEN_OR_IF] = "OR_IF",

    [TOKEN_EOF] = "EOF",
};

const char *token_type_name(token_type_t t)
{
    if (t < 0 || t >= sizeof(token_type_names) / sizeof(token_type_names[0]))
    {
        return "UNKNOWN";
    }
    return token_type_names[t];
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
