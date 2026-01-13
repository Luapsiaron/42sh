#include <stdlib.h>

#include "../lexer/token/token.h"
#include "../utils/str/str.h"
#include "parser_internal.h"

ast_t *parse_simple_command(parser_t *p)
{
    if (peek(p) != TOKEN_WORD)
    {
        return NULL;
    }

    size_t i = 0;
    size_t capacity = 16;

    char **argv = calloc(capacity, sizeof(char *));
    if (!argv)
    {
        return NULL;
    }

    // echo if then fi else elif
    while (peek(p) == TOKEN_WORD)
    {
        if(i+1 >= capacity)
        {
            capacity *= 2;
            char **new_argv = realloc(argv, capacity * sizeof(char *));
            if (!new_argv)
            {
                free_argv(argv);
                return NULL;
            }
            for(size_t j = i; j < capacity; j++)
            {
                new_argv[j] = NULL;
            }
            argv = new_argv;
        }

        argv[i] = xstrdup(p->current_token->lexeme);
        if (!argv[i])
        {
            free_argv(argv);
            return NULL;
        }
        i++;
        pop(p);

        if(i >= 15)
        {
            char **new_argv = realloc(argv, (sizeof(argv) * 2) * sizeof(char *));
            if (!new_argv)
            {
                free_argv(argv);
                return NULL;
            }
            argv = new_argv;
        }
    }
    argv[i] = NULL;

    return ast_cmd_init(argv);
}
