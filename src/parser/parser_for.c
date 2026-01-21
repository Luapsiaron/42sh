#include "parser_internal.h"

static const enum token_type END_TOKENS_DONE[] = { TOKEN_DONE };

static struct ast *parse_for_body(struct parser *p)
{
    return parse_compound_list(
        p, END_TOKENS_DONE, sizeof(END_TOKENS_DONE) / sizeof(*END_TOKENS_DONE));
}

static int is_token_word_in(const struct parser *p)
{
    return peek((struct parser *)p) == TOKEN_WORD && p->current_token && p->current_token->lexeme
           && strcmp(p->current_token->lexeme, "in") == 0;
}

static struct ast *parse_for_name(struct parser *p)
{
    if (peek(p) != TOKEN_WORD)
    {
        return NULL;
    }
    struct ast *name = ast_cmd_init(NULL);
    if (!name)
    {
        return NULL;
    }
    char **argv = calloc(2, sizeof(char *));
    if (!argv)
    {
        ast_free(name);
        return NULL;
    }
    argv[0] = xstrdup(p->current_token->lexeme);
    if (!argv[0])
    {
        free(argv);
        ast_free(name);
        return NULL;
    }
    argv[1] = NULL;
    name->data.ast_cmd.argv = argv;
    pop(p);

    p->lexer.condition = LEXER_NORMAL;
    return name;
}

/*
    Parse the optional "in ..." part of the for statement
*/
static struct ast *parse_for_in_words(struct parser *p)
{
    if(peek(p) != TOKEN_IN && !is_token_word_in(p))
    {
        return NULL;
    }

    pop(p);

    struct ast *list = ast_cmd_init(NULL);
    if (!list)
    {
        return NULL;
    }

    size_t capacity = 8;
    size_t i = 0;
    char **argv = calloc(capacity, sizeof(char *));
    if (!argv)
    {
        ast_free(list);
        return NULL;
    }
    while(peek(p) == TOKEN_WORD || peek(p) == TOKEN_ASSIGNMENT_WORD)
    {
        if (i + 1 >= capacity)
        {
            capacity *= 2;
            char **new_argv = realloc(argv, capacity * sizeof(char *));
            if (!new_argv)
            {
                goto error;
            }
            for(size_t j = i; j < capacity; ++j)
            {
                new_argv[j] = NULL;
            }
            argv = new_argv;
        }
        argv[i] = xstrdup(p->current_token->lexeme);
        if (!argv[i])
        {
            goto error;
        }
        ++i;
        argv[i] = NULL;
        pop(p);
    }

    if(!remove_separator(p))
    {
        goto error;
    }

    list->data.ast_cmd.argv = argv;
    return list;

error:
    free_argv(argv);
    ast_free(list);
    return NULL;
}

/*
    Parse a for statement
    Grammar: for_command = 'for' WORD 'in' WORD { WORD } 'do' compound_list
   'done'

    1. Consumes the initial TOKEN_FOR
    2. Parses the first argument (the loop variable) as a simple command
    3. Optionally parses the "in" condition part, which may include multiple
   words
    4. Consumes the TOKEN_DO
    5. Parses the loop body as a compound list until TOKEN_DONE
    6. Constructs and returns an AST_FOR node with the parsed components
    7. On error, frees any allocated AST nodes and returns NULL
*/
struct ast *parse_for(struct parser *p)
{
    pop(p);

    struct ast *first_arg = parse_for_name(p);
    struct ast *second_arg = NULL;
    if (!first_arg)
    {
        goto error;
    }

    skip_newlines(p);

    if(peek(p) == TOKEN_SEMICOLON)
    {
        pop(p);
    }
    else
    {
        second_arg = parse_for_in_words(p);
        if (!second_arg)
        {
            goto error;
        }
    }
    skip_newlines(p);

    if (peek(p) != TOKEN_DO)
    {
        goto error;
    }
    pop(p);

    struct ast *body = parse_for_body(p);
    if (!body)
    {
        goto error;
    }

    if (peek(p) != TOKEN_DONE)
    {
        ast_free(body);
        goto error;
    }
    pop(p);

    return ast_for_init(first_arg, second_arg, body);

error:
    ast_free(first_arg);
    ast_free(second_arg);
    return NULL;
}
