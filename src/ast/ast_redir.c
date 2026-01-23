#include "ast.h"

struct ast *ast_redir_init(int io_number, enum redir_type type,
                           const char *word, struct ast *next)
{
    struct ast *new = ast_init(AST_REDIR);
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

int ast_redir_append(struct ast *cmd, struct ast *redir)
{
    if (!cmd || cmd->type != AST_CMD || !redir || redir->type != AST_REDIR)
    {
        return 0;
    }

    struct ast **head = &cmd->data.ast_cmd.redirs;
    if (!*head)
    {
        *head = redir;
        return 1;
    }

    struct ast *cur = *head;
    while (cur->data.ast_redir.next)
    {
        cur = cur->data.ast_redir.next;
    }
    cur->data.ast_redir.next = redir;
    return 1;
}