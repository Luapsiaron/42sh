#include "parser_internal.h"

ast_t *parse_condition(parser_t *p)
{
    skip_semicolon_newline(p);

    if (peek(p) == TOKEN_THEN || peek(p) == TOKEN_EOF)
    {
        return NULL;
    }

    ast_t *first = parse_simple_command(p);
    if (!first)
    {
        return NULL;
    }

    ast_t *list = ast_list_init(NULL, first);
    if (!list)
    {
        ast_free(first);
        return NULL;
    }

    ast_t *tmp = list;

    while (peek(p) != TOKEN_THEN)
    {
        if (peek(p) == TOKEN_EOF)
        {
            ast_free(list);
            return NULL;
        }

        skip_semicolon_newline(p);

        if (peek(p) == TOKEN_THEN)
        {
            break;
        }

        ast_t *cmd = parse_simple_command(p);
        if (!cmd)
        {
            ast_free(list);
            return NULL;
        }
        tmp->data.ast_list.next = ast_list_init(NULL, cmd);
        tmp = tmp->data.ast_list.next;
        if (!tmp)
        {
            ast_free(cmd);
            ast_free(list);
            return NULL;
        }

        skip_semicolon_newline(p);
    }
    return list;
}

/*
ast_t *parse_then(parser_t *p)
{
    skip_semicolon_newline(p);

    ast_t *list = ast_list_init(NULL, parse_simple_command(p));

    ast_t *tmp = list;

    while (peek(p) != TOKEN_FI && peek(p) != TOKEN_ELSE
           && peek(p) != TOKEN_ELIF)
    {
        if (peek(p) == TOKEN_EOF)
        {
            return NULL;
        }
        tmp->data.ast_list.next = ast_list_init(NULL, parse_simple_command(p));
        tmp = tmp->data.ast_list.next;
        skip_semicolon_newline(p);
    }
    return list;
}
*/

// sauv type token, TOKEN_THEN
// switch switch case TOKEN_THEN: peek(p) == TOKEN_FI || peek(p) == TOKEN_ELSE
// || peek(p) == TOKEN_ELIF || peek(p) == TOKEN_EOF

ast_t *parse_then(parser_t *p)
{
    skip_semicolon_newline(p);

    if (peek(p) == TOKEN_FI || peek(p) == TOKEN_ELSE || peek(p) == TOKEN_ELIF
        || peek(p) == TOKEN_EOF)
    {
        return NULL;
    }

    ast_t *first = parse_simple_command(p);
    if (!first)
    {
        return NULL;
    }

    ast_t *list = ast_list_init(NULL, first);
    if (!list)
    {
        ast_free(first);
        return NULL;
    }

    ast_t *tmp = list;

    while (peek(p) != TOKEN_FI && peek(p) != TOKEN_ELSE
           && peek(p) != TOKEN_ELIF)
    {
        if (peek(p) == TOKEN_EOF)
        {
            ast_free(list);
            return NULL;
        }

        skip_semicolon_newline(p);

        if (peek(p) == TOKEN_FI || peek(p) == TOKEN_ELSE
            || peek(p) == TOKEN_ELIF)
        {
            break;
        }

        ast_t *cmd = parse_simple_command(p);
        if (!cmd)
        {
            ast_free(list);
            return NULL;
        }

        tmp->data.ast_list.next = ast_list_init(NULL, cmd);
        tmp = tmp->data.ast_list.next;
        if (!tmp)
        {
            ast_free(cmd);
            ast_free(list);
            return NULL;
        }

        skip_semicolon_newline(p);
    }

    return list;
}

ast_t *parse_else(parser_t *p)
{
    skip_semicolon_newline(p);

    if (peek(p) == TOKEN_FI || peek(p) == TOKEN_EOF)
    {
        return NULL;
    }

    ast_t *first = parse_simple_command(p);
    if (!first)
    {
        return NULL;
    }
    ast_t *list = ast_list_init(NULL, first);
    if (!list)
    {
        ast_free(first);
        return NULL;
    }

    ast_t *tmp = list;

    while (peek(p) != TOKEN_FI)
    {
        if (peek(p) == TOKEN_EOF)
        {
            ast_free(list);
            return NULL;
        }

        skip_semicolon_newline(p);

        if (peek(p) == TOKEN_FI)
        {
            break;
        }

        ast_t *cmd = parse_simple_command(p);
        if (!cmd)
        {
            ast_free(list);
            return NULL;
        }
        tmp->data.ast_list.next = ast_list_init(NULL, cmd);
        tmp = tmp->data.ast_list.next;
        if (!tmp)
        {
            ast_free(cmd);
            ast_free(list);
            return NULL;
        }
        skip_semicolon_newline(p);
    }
    return list;
}

ast_t *parse_elif_command(parser_t *p)
{
    if (peek(p) == TOKEN_ELIF)
    {
        return parse_elif(p);
    }
    return NULL;
}

ast_t *parse_elif(parser_t *p)
{
    pop(p);

    skip_semicolon_newline(p);
    ast_t *condition = parse_condition(p);
    if (!condition)
    {
        return NULL;
    }

    skip_semicolon_newline(p);
    if (peek(p) != TOKEN_THEN)
    {
        ast_free(condition);
        return NULL;
    }
    pop(p);

    ast_t *then_body = parse_then(p);
    if (!then_body)
    {
        ast_free(condition);
        return NULL;
    }

    skip_semicolon_newline(p);

    ast_t *elif_body = NULL;
    if (peek(p) == TOKEN_ELIF)
    {
        elif_body = parse_elif_command(p);
        if (!elif_body)
        {
            ast_free(condition);
            ast_free(then_body);
            return NULL;
        }
    }

    skip_semicolon_newline(p);

    ast_t *else_body = NULL;
    if (peek(p) == TOKEN_ELSE)
    {
        pop(p);
        skip_semicolon_newline(p);
        else_body = parse_else(p);
        if (!else_body)
        {
            ast_free(condition);
            ast_free(then_body);
            return NULL;
        }
    }

    if (elif_body)
    {
        return ast_if_init(condition, then_body, elif_body);
    }
    return ast_if_init(condition, then_body, else_body);
}

static ast_t *parse_if_condition(parser_t *p)
{
    skip_semicolon_newline(p);
    ast_t *condition = parse_condition(p);
    if (!condition)
    {
        return NULL;
    }

    skip_semicolon_newline(p);
    if (peek(p) != TOKEN_THEN)
    {
        ast_free(condition);
        return NULL;
    }
    pop(p);
    return condition;
}

static ast_t *parse_if_then(parser_t *p, ast_t *condition)
{
    skip_semicolon_newline(p);
    ast_t *then_body = parse_then(p);
    if (!then_body)
    {
        ast_free(condition);
        return NULL;
    }
    return then_body;
}

ast_t *parse_if(parser_t *p)
{
    pop(p);

    ast_t *condition = parse_if_condition(p);
    if (!condition)
    {
        return NULL;
    }

    ast_t *then_body = parse_if_then(p, condition);
    if (!then_body)
    {
        ast_free(condition);
        return NULL;
    }

    skip_semicolon_newline(p);

    ast_t *elif_body = NULL;
    if (peek(p) == TOKEN_ELIF)
    {
        elif_body = parse_elif_command(p);
        skip_semicolon_newline(p);
        if (!elif_body)
        {
            ast_free(condition);
            ast_free(then_body);
            return NULL;
        }
    }

    skip_semicolon_newline(p);

    ast_t *else_body = NULL;
    if (peek(p) == TOKEN_ELSE)
    {
        pop(p);
        skip_semicolon_newline(p);
        else_body = parse_else(p);
        if (!else_body)
        {
            ast_free(condition);
            ast_free(then_body);
            return NULL;
        }
    }
    skip_semicolon_newline(p);
    if (peek(p) != TOKEN_FI)
    {
        ast_free(condition);
        ast_free(then_body);
        ast_free(else_body);
        return NULL;
    }
    pop(p);

    if (elif_body)
    {
        return ast_if_init(condition, then_body, elif_body);
    }
    return ast_if_init(condition, then_body, else_body);
}
