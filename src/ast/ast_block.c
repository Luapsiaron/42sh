#include "ast.h"

struct ast *ast_block_init(struct ast *body)
{
    struct ast *new = ast_init(AST_BLOCK);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_block.body = body;
    return new;
}

struct ast *ast_funcdec_init(const char *name, struct ast *body,
                             struct ast *redirs)
{
    struct ast *new = ast_init(AST_FUNCDEC);
    if (!new)
    {
        return NULL;
    }

    if (name)
    {
        new->data.ast_funcdec.name = xstrdup(name);
        if (!new->data.ast_funcdec.name)
        {
            free(new);
            return NULL;
        }
    }
    new->data.ast_funcdec.body = body;
    new->data.ast_funcdec.redirs = redirs;
    return new;
}

struct ast *ast_redirwrap_init(struct ast *shell_command,
                               struct ast *redirections)
{
    struct ast *new = ast_init(AST_REDIRWRAP);
    if (!new)
    {
        return NULL;
    }

    new->data.ast_redirwrap.shell_command = shell_command;
    new->data.ast_redirwrap.redirections = redirections;
    return new;
}
