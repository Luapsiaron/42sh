#include "parser.h"
#include "parser_internal.h"

/*
    Parse a list of commands separated by semicolons or newlines
    Grammar: list = and_or { separator and_or }

    Examples:
    - echo a
    - echo a ; echo b
    - echo a
      echo b
*/
struct ast *parse_list(struct parser *p)
{
    skip_newlines(p);
    if (peek(p) == TOKEN_SEMICOLON)
    {
        parse_set_error();
        return NULL;
    }

    struct ast *child = parse_and_or(p);
    if (!child)
    {
        if (!parse_error_occurred())
        {
            parse_set_error();
        }
        return NULL;
    }
    struct ast *head = ast_list_init(NULL, child);
    if (!head)
    {
        goto error;
    }

    struct ast *tail = head;

    while (1)
    {
        if (parse_error_occurred())
        {
            goto error;
        }
        if (!remove_separator(p))
        {
            break;
        }
        if (parse_error_occurred())
        {
            goto error;
        }
        if (peek(p) == TOKEN_SEMICOLON)
        {
            parse_set_error();
            goto error;
        }
        if (peek(p) == TOKEN_EOF)
        {
            break;
        }

        struct ast *next_child = parse_and_or(p);
        if (!next_child)
        {
            if (!parse_error_occurred())
            {
                parse_set_error();
            }
            goto error;
        }

        struct ast *next_list = ast_list_init(NULL, next_child);
        if (!next_list)
        {
            ast_free(next_child);
            goto error;
        }

        tail->data.ast_list.next = next_list;
        tail = next_list;
    }
    return head;

error:
    ast_free(head);
    return NULL;
}
