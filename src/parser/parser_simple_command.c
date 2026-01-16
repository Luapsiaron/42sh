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

static int is_prefix_token(token_type_t type)
{
    return type == TOKEN_IONUMBER || is_redirection_token(type)
        || type == TOKEN_ASSIGNMENT_WORD;
}

static int is_element_token(token_type_t type)
{
    return type == TOKEN_WORD || is_prefix_token(type);
}

static int cmd_has_prefix(const ast_t *cmd)
{
    return cmd && (cmd->data.ast_cmd.redirs || cmd->data.ast_cmd.assignments);
}

static int parse_cmd_prefix(parser_t *p, ast_t *cmd)
{
    while (1)
    {
        if (peek(p) == TOKEN_ASSIGNMENT_WORD)
        {
            ast_t *assignment = parse_assignment(p);
            if (!assignment || !ast_assignment_append(cmd, assignment))
            {
                ast_free(assignment);
                return 0;
            }
            continue;
        }

        if (peek(p) == TOKEN_IONUMBER || is_redirection_token(peek(p)))
        {
            ast_t *redir = parse_redirection(p);
            if (!redir || !ast_redir_append(cmd, redir))
            {
                ast_free(redir);
                return 0;
            }
            continue;
        }
        break;
    }

    return -1;
}

static int push_argv_word(parser_t *p, char ***argv, size_t *i,
                          size_t *capacity)
{
    if (!fill_argv(argv, i, capacity, p->current_token->lexeme))
    {
        return 0;
    }
    pop(p);
    return 1;
}

static char **parse_cmd_argv(parser_t *p, ast_t *cmd)
{
    size_t i = 0;
    size_t capacity = 16;

    char **argv = calloc(capacity, sizeof(char *));
    if (!argv)
    {
        return NULL;
    }

    while (is_element_token(peek(p)))
    {
        if (peek(p) == TOKEN_WORD || peek(p) == TOKEN_ASSIGNMENT_WORD)
        {
            if (!push_argv_word(p, &argv, &i, &capacity))
            {
                free_argv(argv);
                return NULL;
            }
            continue;
        }

        ast_t *redir = parse_redirection(p);
        if (!redir || !ast_redir_append(cmd, redir))
        {
            free_argv(argv);
            ast_free(redir);
            return NULL;
        }
    }

    argv[i] = NULL;

    if (i == 0)
    {
        free_argv(argv);
        return NULL;
    }
    return argv;
}

ast_t *parse_simple_command(parser_t *p)
{
    ast_t *cmd = ast_cmd_init(NULL);
    if (!cmd)
    {
        return NULL;
    }

    if (!parse_cmd_prefix(p, cmd))
    {
        goto error;
    }

    if (peek(p) != TOKEN_WORD)
    {
        if (!cmd_has_prefix(cmd))
        {
            goto error;
        }
        return cmd;
    }

    cmd->data.ast_cmd.argv = parse_cmd_argv(p, cmd);
    if (!cmd->data.ast_cmd.argv)
    {
        goto error;
    }
    return cmd;

error:
    ast_free(cmd);
    return NULL;
}
