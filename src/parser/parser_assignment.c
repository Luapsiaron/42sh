#include <stdlib.h>
#include <string.h>

#include "../utils/str/str.h"
#include "parser_internal.h"

/*
    Parse an assignment word
    Grammar: NAME = VALUE

    The lexer garantees that the token matches pattern [a-zA-Z_][a-zA-Z0-9_]*=.*
    The parser does:
    1. Split the lexeme at the '=' character
    2. Create an AST_ASSIGNMENT node with variable name and value

    Example:
    - ASSIGNMENT_WORD("PATH=/bin")
      AST_ASSIGNMENT
        var = "PATH"
        value = "/bin"
*/
struct ast *parse_assignment(struct parser *p)
{
    if (peek(p) != TOKEN_ASSIGNMENT_WORD)
    {
        return NULL;
    }

    const char *s = p->current_token->lexeme;
    char *equal_sign = strchr(s, '=');
    if (!equal_sign)
    {
        return NULL;
    }

    size_t var_name_len = equal_sign - s;
    char *var_name = xstrndup(s, var_name_len);
    if (!var_name)
    {
        return NULL;
    }

    const char *value = equal_sign + 1;

    struct ast *assignment = ast_assignment_init(var_name, value);
    free(var_name);
    if (!assignment)
    {
        return NULL;
    }

    pop(p);
    return assignment;
}