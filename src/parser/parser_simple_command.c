#include <stdlib.h>

#include "../lexer/token/token.h"
#include "../utils/str/str.h"
#include "parser_internal.h"

static int fill_argv(char ***argv, size_t *i, size_t *capacity, const char *s)
{
    if (*i + 1 >= *capacity)
    {
        *capacity *= 2;
        char **new_argv = realloc(*argv, *capacity * sizeof(char *));
        if (!new_argv)
        {
            return 0;
        }
        for (size_t j = *i; j < *capacity; ++j)
        {
            new_argv[j] = NULL;
        }
        *argv = new_argv;
    }

    (*argv)[*i] = xstrdup(s);
    if (!(*argv)[*i])
    {
        return 0;
    }
    (*i)++;
    (*argv)[*i] = NULL;
    return 1;
}
ast_t *parse_simple_command(parser_t *p)
{
    ast_t *cmd = ast_cmd_init(NULL);
    if (!cmd)
    {
        return NULL;
    }

    while (peek(p) == TOKEN_IONUMBER || is_redirection_token(peek(p)))
    {
        ast_t *redir = parse_redirection(p);
        if (!redir || !ast_redir_append(cmd, redir))
        {
            ast_free(cmd);
            ast_free(redir);
            return NULL;
        }
    }

    size_t i = 0;
    size_t capacity = 16;

    char **argv = calloc(capacity, sizeof(char *));
    if (!argv)
    {
        ast_free(cmd);
        return NULL;
    }

    while (peek(p) == TOKEN_WORD || peek(p) == TOKEN_IONUMBER
           || is_redirection_token(peek(p)))
    {
        if (peek(p) == TOKEN_WORD)
        {
            if (!fill_argv(&argv, &i, &capacity, p->current_token->lexeme))
            {
                ast_free(cmd);
                free_argv(argv);
                return NULL;
            }
            pop(p);
            continue;
        }

        ast_t *redir = parse_redirection(p);
        if (!redir || !ast_redir_append(cmd, redir))
        {
            ast_free(cmd);
            free_argv(argv);
            ast_free(redir);
            return NULL;
        }
    }

    if (i == 0)
    {
        free_argv(argv);
        argv = NULL;
    }
    else
    {
        argv[i] = NULL;
    }

    if (!argv && cmd->data.ast_cmd.redirs == NULL)
    {
        ast_free(cmd);
        return NULL;
    }
    cmd->data.ast_cmd.argv = argv;
    return cmd;
}
