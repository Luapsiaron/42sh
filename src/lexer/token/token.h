#ifndef TOKEN_H
#define TOKEN_H

typedef enum token_type
{
    // POSIX Reserved word
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_ELIF,
    TOKEN_FI,

    // Separators
    TOKEN_SEMICOLON,
    TOKEN_NEWLINE,

    TOKEN_WORD, // ls, echo etc

    // Operators
    TOKEN_PIPE,
    TOKEN_REDIRECT, // > or >>

    TOKEN_NEGATION, // !

    TOKEN_AND_IF, // &&
    TOKEN_OR_IF,  // ||
    
    // End of input
    TOKEN_EOF
} token_type_t;

typedef struct token
{
    token_type_t type;
    char *lexeme; // token text
                  // Only for TOKEN_WORD, NULL for others
} token_t;

// Allocate and initialize a new token
// type: type of the token
// value: lexeme of the token (Word copied with strdup or NULL)
// The token must be free with token_free() at the end
token_t *token_new(token_type_t type, const char *value);

void token_free(token_t *t);

// Return a string correspond to the token type
// Use for debugging
const char *token_type_name(token_type_t t);

// Check if it corresponds to a shell reserved word (If, else, etc.)
// s: input string
// out: output token type if s is a reserverd word
// Return bool if s is a reserved word or not
int token_is_reserved_word(const char *s, token_type_t *out);

// DEBUG Function: print the token
void token_printer(const token_t *t);

#endif /* ! TOKEN_H */
