#include "parser_internal.h"

ast_t *parse_list(parser_t *p)
{
    skip_semicolon_newline(p);

    ast_t *child = parser_pipeline(p);
    if (!child)
    {
        return NULL;
    }
    ast_t *head = ast_list_init(NULL, child);
    if (!head)
    {
        ast_free(child);
        return NULL;
    }

    ast_t *tail = head;

    while (1)
    {
        if (!remove_separator(p))
        {
            break;
        }

        if (peek(p) == TOKEN_EOF)
        {
            break;
        }

        ast_t *next_child = parser_pipeline(p);
        if (!next_child)
        {
            ast_free(head);
            return NULL;
        }

        ast_t *next_list = ast_list_init(NULL, next_child);
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
