#ifndef TOKEN_H
#define TOKEN_H

enum token_type 
{
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_ELIF,
    TOKEN_FI,
    TOKEN_SEMICOLON,
    TOKEN_NEWLINE,
    TOKEN_WORD, // ls, echo etc
    TOKEN_PIPE,
    TOKEN_REDIRECT, // > or >>
    TOKEN_EOF
};

struct token 
{
    token_type type;
    char *value; // token text
    struct token *next;
};

#endif /* ! TOKEN_H */