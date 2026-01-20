#include "parser_internal.h"

struct ast *parse_list(struct parser *p)
{
    skip_newlines(p);
    if (peek(p) == TOKEN_SEMICOLON)
    {
        return NULL;
    }

    struct ast *child = parse_and_or(p);
    if (!child)
    {
        return NULL;
    }
    struct ast *head = ast_list_init(NULL, child);
    if (!head)
    {
        ast_free(child);
        return NULL;
    }

    struct ast *tail = head;

    while (1)
    {
        if (!remove_separator(p))
        {
            break;
        }
        if (peek(p) == TOKEN_SEMICOLON)
        {
            ast_free(head);
            return NULL;
        }
        if (peek(p) == TOKEN_EOF)
        {
            break;
        }

        struct ast *next_child = parse_and_or(p);
        if (!next_child)
        {
            ast_free(head);
            return NULL;
        }

        struct ast *next_list = ast_list_init(NULL, next_child);
        if (!next_list)
        {
            ast_free(next_child);
            ast_free(head);
            return NULL;
        }

        tail->data.ast_list.next = next_list;
        tail = next_list;
    }
    return head;
}
