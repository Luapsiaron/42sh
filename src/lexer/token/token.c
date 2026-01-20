#include "token.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../../utils/str/str.h"

struct token *token_new(enum token_type type, const char *value)
{
    struct token *t = calloc(1, sizeof(*t));
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

void token_free(struct token *t)
{
    if (!t)
    {
        return;
    }
    free(t->lexeme);
    free(t);
}

static const struct reserved_word reserved_words[] = {
    { "if", TOKEN_IF },       { "then", TOKEN_THEN },   { "else", TOKEN_ELSE },
    { "elif", TOKEN_ELIF },   { "fi", TOKEN_FI },       { "for", TOKEN_FOR },
    { "while", TOKEN_WHILE }, { "until", TOKEN_UNTIL }, { "in", TOKEN_IN },
    { "do", TOKEN_DO },       { "done", TOKEN_DONE },
};

int token_is_reserved_word(const char *s, enum token_type *out_type)
{
    size_t n = sizeof(reserved_words) / sizeof(reserved_words[0]);
    for (size_t i = 0; i < n; i++)
    {
        if (strcmp(s, reserved_words[i].word) == 0)
        {
            if (out_type)
            {
                *out_type = reserved_words[i].type;
            }
            return 1;
        }
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

const char *token_type_name(enum token_type t)
{
    if (t < 0 || t >= sizeof(token_type_names) / sizeof(token_type_names[0]))
    {
        return "UNKNOWN";
    }
    return token_type_names[t];
}

void token_printer(const struct token *t)
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
