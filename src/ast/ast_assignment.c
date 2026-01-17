#include "ast.h"

ast_t *ast_assignment_init(const char *var, const char *value)
{
    ast_t *new = ast_init(AST_ASSIGNMENT);
    if (!new)
    {
        return NULL;
    }

    if (var)
    {
        new->data.ast_assignment.var_name = xstrdup(var);
        if (!new->data.ast_assignment.var_name)
        {
            free(new);
            return NULL;
        }
    }
    if (value)
    {
        new->data.ast_assignment.value = xstrdup(value);
        if (!new->data.ast_assignment.value)
        {
            free(new->data.ast_assignment.var_name);
            free(new);
            return NULL;
        }
    }
    new->data.ast_assignment.next = NULL;
    return new;
}

int ast_assignment_append(ast_t *cmd, ast_t *assignment)
{
    if (!cmd || cmd->type != AST_CMD || !assignment
        || assignment->type != AST_ASSIGNMENT)
    {
        return 0;
    }

    ast_t **head = &cmd->data.ast_cmd.assignments;
    if (!*head)
    {
        *head = assignment;
        return 1;
    }

    ast_t *cur = *head;
    while (cur->data.ast_assignment.next)
    {
        cur = cur->data.ast_assignment.next;
    }
    cur->data.ast_assignment.next = assignment;
    return 1;
}