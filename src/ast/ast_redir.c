#include "ast.h"

ast_t *ast_redir_init(int io_number, redir_type_t type, const char *word,
                      ast_t *next)
{
    ast_t *new = ast_init(AST_REDIR);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_redir.io_number = io_number;
    new->data.ast_redir.type = type;

    if (word)
    {
        new->data.ast_redir.word = xstrdup(word);
        if (!new->data.ast_redir.word)
        {
            free(new);
            return NULL;
        }
    }
    new->data.ast_redir.next = next;
    return new;
}

int ast_redir_append(ast_t *cmd, ast_t *redir)
{
    if (!cmd || cmd->type != AST_CMD || !redir || redir->type != AST_REDIR)
    {
        return 0;
    }

    ast_t **head = &cmd->data.ast_cmd.redirs;
    if (!*head)
    {
        *head = redir;
        return 1;
    }

    ast_t *cur = *head;
    while (cur->data.ast_redir.next)
    {
        cur = cur->data.ast_redir.next;
    }
    cur->data.ast_redir.next = redir;
    return 1;
}