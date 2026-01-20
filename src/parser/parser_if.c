#include "parser_internal.h"

/*
    if <condition>; then <then_body>; [elif <condition>; then <then_body>;]*
   [else <else_body>;] fi

    Example: if false; then echo A; elif true; then echo B; else echo C; fi
    AST representation:
    IF --> CONDITION (LIST) --> false
        THEN (LIST)     --> echo A
        ELIF (IF)       --> CONDITION (LIST) --> true
                            THEN (LIST)      --> echo B
        ELSE (LIST)     --> echo C
*/

/*
    End tokens for different parts of the if statement
    Used to determine when to stop parsing a specific part
    (e.g., condition, then body, else body)
*/
static const enum token_type END_TOKENS_CONDITION[] = { TOKEN_THEN };
static const enum token_type END_TOKENS_THEN[] = { TOKEN_FI, TOKEN_ELSE,
                                                TOKEN_ELIF };
static const enum token_type END_TOKENS_ELSE[] = { TOKEN_FI };

/*
    Structure to hold the elif and else bodies
*/
struct elif_else_body
{
    struct ast *elif_body;
    struct ast *else_body;
};

/* Forward declaration */
static struct ast *parse_elif_command(struct parser *p);

/*
    Helper function to check if a token is a stop token
    Exemple: end_token = {TOKEN8_FI, TOKEN_ELSE, TOKEN_ELIF}, end_token_count =
   3 is_stop_token(TOKEN_ELSE, end_token, end_token_count) -> 1
    is_stop_token(TOKEN_THEN, end_token, end_token_count) -> 0
*/
static int is_stop_token(enum token_type token, const enum token_type *end_token,
                         size_t end_token_count)
{
    for (size_t i = 0; i < end_token_count; i++)
    {
        if (token == end_token[i])
        {
            return 1;
        }
    }
    return 0;
}

/*
    Helper function to expect a specific token
    If the next token matches the expected token, it pops it and returns 1
    Otherwise, it returns 0
    Used to simplify token expectation checks in the IF grammar parsing
*/
static int expect_token(struct parser *p, enum token_type expected)
{
    if (peek(p) != expected)
    {
        return 0;
    }
    pop(p);
    return 1;
}

/*
    Parse a compound list until one of the specified end tokens is encountered
    Returns a list of commands or NULL on error
*/
struct ast *parse_compound_list(struct parser *p, const enum token_type *end_token,
                           size_t end_token_count)
{
    skip_newlines(p);

    /* Check if we've reached an end token or EOF (Means invalid condition like
     * 'if then')*/
    if (is_stop_token(peek(p), end_token, end_token_count)
        || peek(p) == TOKEN_EOF)
    {
        return NULL;
    }

    struct ast *first = parse_and_or(p);
    if (!first)
    {
        return NULL;
    }
    struct ast *list = ast_list_init(NULL, first);
    if (!list)
    {
        ast_free(first);
        return NULL;
    }

    struct ast *tmp = list;

    while (1)
    {
        if (is_stop_token(peek(p), end_token, end_token_count)
            || peek(p) == TOKEN_EOF)
        {
            goto error;
        }

        if (!remove_separator(p))
        {
            goto error;
        }

        if (is_stop_token(peek(p), end_token, end_token_count))
        {
            break;
        }

        if (peek(p) == TOKEN_EOF)
        {
            goto error;
        }

        struct ast *cmd = parse_and_or(p);
        if (!cmd)
        {
            goto error;
        }
        tmp->data.ast_list.next = ast_list_init(NULL, cmd);
        tmp = tmp->data.ast_list.next;
        if (!tmp)
        {
            ast_free(cmd);
            goto error;
        }
    }
    return list;

error:
    ast_free(list);
    return NULL;
}

/*
    Parse the condition part of the if statement until TOKEN_THEN
*/
static struct ast *parse_condition(struct parser *p)
{
    return parse_compound_list(p, END_TOKENS_CONDITION,
                               sizeof(END_TOKENS_CONDITION)
                                   / sizeof(*END_TOKENS_CONDITION));
}

/*
    Parse the then part of the if statement until TOKEN_FI, TOKEN_ELSE, or
   TOKEN_ELIF
*/
static struct ast *parse_then(struct parser *p)
{
    return parse_compound_list(
        p, END_TOKENS_THEN, sizeof(END_TOKENS_THEN) / sizeof(*END_TOKENS_THEN));
}

/*
    Parse the else part of the if statement until TOKEN_FI
*/
static struct ast *parse_else(struct parser *p)
{
    return parse_compound_list(
        p, END_TOKENS_ELSE, sizeof(END_TOKENS_ELSE) / sizeof(*END_TOKENS_ELSE));
}

/*
    Parse the elif and else bodies of the if statement
    After parsing the then body, this function checks for the presence of elif
   and else parts
    - If an elif is found, it parses it and assigns it to body->elif_body
    - If an else is found, it parses it and assigns it to body->else_body
    If any parsing fails, it frees the previously allocated AST nodes and
   returns 0
*/
static int parse_elif_else_body(struct parser *p, struct elif_else_body *body,
                                struct ast *condition, struct ast *then_body)
{
    skip_newlines(p);

    body->elif_body = NULL;
    body->else_body = NULL;

    if (peek(p) == TOKEN_ELIF)
    {
        body->elif_body = parse_elif_command(p);
        if (!body->elif_body)
        {
            ast_free(condition);
            ast_free(then_body);
            return 0;
        }
    }

    skip_newlines(p);

    if (peek(p) == TOKEN_ELSE)
    {
        pop(p);
        skip_newlines(p);
        body->else_body = parse_else(p);
        if (!body->else_body)
        {
            ast_free(body->elif_body);
            ast_free(condition);
            ast_free(then_body);
            return 0;
        }
    }
    return 1;
}

/*
    Parse an elif command
    elif <condition>; then <then_body>; [elif <condition>; then <then_body>;]*
   [else <else_body>;] Does not delete the FI token at the end Is deleted by
   parse_if
*/
static struct ast *parse_elif(struct parser *p)
{
    pop(p);

    struct ast *condition = parse_condition(p);
    if (!condition)
    {
        return NULL;
    }

    skip_newlines(p);
    if (!expect_token(p, TOKEN_THEN))
    {
        ast_free(condition);
        return NULL;
    }

    struct ast *then_body = parse_then(p);
    if (!then_body)
    {
        ast_free(condition);
        return NULL;
    }

    struct elif_else_body body;
    if (!parse_elif_else_body(p, &body, condition, then_body))
    {
        return NULL;
    }

    if (body.elif_body)
    {
        return ast_if_init(condition, then_body, body.elif_body);
    }
    return ast_if_init(condition, then_body, body.else_body);
}

static struct ast *parse_elif_command(struct parser *p)
{
    if (peek(p) == TOKEN_ELIF)
    {
        return parse_elif(p);
    }
    return NULL;
}

/*
    Parse an if statement
    - Consumes the initial TOKEN_IF
    - Parses the condition part until TOKEN_THEN
    - Expects and consumes TOKEN_THEN
    - Parses the then body until TOKEN_FI, TOKEN_ELSE, or TOKEN_ELIF
    - Parses any elif and else bodies
    - Expects and consumes TOKEN_FI
    - Constructs and returns the AST node for the if statement
    - On any error, frees allocated AST nodes and returns NULL
*/
struct ast *parse_if(struct parser *p)
{
    pop(p);

    struct ast *condition = parse_condition(p);
    if (!condition)
    {
        return NULL;
    }

    skip_newlines(p);
    if (!expect_token(p, TOKEN_THEN))
    {
        ast_free(condition);
        return NULL;
    }

    struct ast *then_body = parse_then(p);
    if (!then_body)
    {
        ast_free(condition);
        return NULL;
    }

    struct elif_else_body body;
    if (!parse_elif_else_body(p, &body, condition, then_body))
    {
        return NULL;
    }

    skip_newlines(p);
    if (!expect_token(p, TOKEN_FI))
    {
        ast_free(condition);
        ast_free(then_body);
        ast_free(body.elif_body);
        ast_free(body.else_body);
        return NULL;
    }

    if (body.elif_body)
    {
        return ast_if_init(condition, then_body, body.elif_body);
    }
    return ast_if_init(condition, then_body, body.else_body);
}
