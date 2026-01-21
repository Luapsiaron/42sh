#ifndef TOKEN_H
#define TOKEN_H

enum token_type
{
    // POSIX Reserved word
    TOKEN_IF,
    TOKEN_THEN,
    TOKEN_ELSE,
    TOKEN_ELIF,
    TOKEN_FI,

    // Loop
    TOKEN_FOR,
    TOKEN_WHILE,
    TOKEN_UNTIL,
    TOKEN_IN,
    TOKEN_DO,
    TOKEN_DONE,

    // Separators
    TOKEN_SEMICOLON,
    TOKEN_NEWLINE,

    TOKEN_WORD, // ls, echo etc
    TOKEN_ASSIGNMENT_WORD, // VAR=VALUE (prefix)

    // Operators
    TOKEN_PIPE,
    TOKEN_LESS, // <
    TOKEN_GREAT, // >
    TOKEN_DGREAT, // >>
    TOKEN_CLOBBER, // >|
    TOKEN_LESSAND, // <&
    TOKEN_GREATAND, // >&
    TOKEN_LESSGREAT, // <>
    TOKEN_IONUMBER, // n>

    TOKEN_NEGATION, // !

    TOKEN_AND_IF, // &&
    TOKEN_OR_IF, // ||

    // End of input
    TOKEN_EOF
};

struct token
{
    enum token_type type;
    char *lexeme; // token text
                  // Only for TOKEN_WORD, NULL for others
};

struct reserved_word
{
    const char *word;
    enum token_type type;
};

// Allocate and initialize a new token
// type: type of the token
// value: lexeme of the token (Word copied with strdup or NULL)
// The token must be free with token_free() at the end
struct token *token_new(enum token_type type, const char *value);

void token_free(struct token *t);

// Return a string correspond to the token type
// Use for debugging
const char *token_type_name(enum token_type t);

// Check if it corresponds to a shell reserved word (If, else, etc.)
// s: input string
// out: output token type if s is a reserverd word
// Return bool if s is a reserved word or not
int token_is_reserved_word(const char *s, enum token_type *out);

// DEBUG Function: print the token
void token_printer(const struct token *t);

#endif /* ! TOKEN_H */
