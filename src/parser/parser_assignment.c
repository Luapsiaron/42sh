#include <stdlib.h>
#include <string.h>

#include "../utils/str/str.h"
#include "parser_internal.h"

ast_t *parse_assignment(parser_t *p)
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

    ast_t *assignment = ast_assignment_init(var_name, value);
    free(var_name);
    if (!assignment)
    {
        return NULL;
    }

    pop(p);
    return assignment;
}