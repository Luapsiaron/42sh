#include <stdlib.h>

#include "../lexer/token/token.h"
#include "../utils/str/str.h"
#include "parser_internal.h"

/*
    Append a string to the argv array, resizing if necessary
    Used in parse_simple_command to build the argv list dynamically
*/
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

static int is_prefix_token(enum token_type type)
{
    return type == TOKEN_IONUMBER || is_redirection_token(type)
        || type == TOKEN_ASSIGNMENT_WORD;
}

static int is_element_token(enum token_type type)
{
    return type == TOKEN_WORD || is_prefix_token(type);
}

static int cmd_has_prefix(const struct ast *cmd)
{
    return cmd && (cmd->data.ast_cmd.redirs || cmd->data.ast_cmd.assignments);
}

/*
    Parse the command prefix using their respective parsers
    Grammar: { assignment_word | redirection }
    Returns 1 on success, 0 on failure, -1 if no prefix found
*/
static int parse_cmd_prefix(struct parser *p, struct ast *cmd)
{
    while (1)
    {
        if (peek(p) == TOKEN_ASSIGNMENT_WORD)
        {
            struct ast *assignment = parse_assignment(p);
            if (!assignment || !ast_assignment_append(cmd, assignment))
            {
                ast_free(assignment);
                return 0;
            }
            continue;
        }

        if (peek(p) == TOKEN_IONUMBER || is_redirection_token(peek(p)))
        {
            struct ast *redir = parse_redirection(p);
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

/*
    Push the current token's lexeme into the argv array
    Advances the parser to the next token
*/
static int push_argv_word(struct parser *p, char ***argv, size_t *i,
                          size_t *capacity)
{
    if (!fill_argv(argv, i, capacity, p->current_token->lexeme))
    {
        return 0;
    }
    pop(p);
    return 1;
}

/*
    Parse the argv array for the simple command
    Grammar: WORD { element }
    Returns the argv array or NULL on error
*/
static char **parse_cmd_argv(struct parser *p, struct ast *cmd)
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

        struct ast *redir = parse_redirection(p);
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
/*
    Parse a simple command
    Grammar: simple_command = { assignment_word | redirection } WORD { element }
    Returns the AST node for the simple command or NULL on error

    Valid examples:
    echo hello world
    A=1 B=2 echo ok
    >out echo test
    A=1 >out
    ... etc
*/
struct ast *parse_simple_command(struct parser *p)
{
    struct ast *cmd = ast_cmd_init(NULL);
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
