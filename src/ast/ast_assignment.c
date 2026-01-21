#include "ast.h"

/*
    Initialize an assignment AST node
    Grammar: NAME=VALUE
*/
struct ast *ast_assignment_init(const char *var, const char *value)
{
    struct ast *new = ast_init(AST_ASSIGNMENT);
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

/*
    Append an assignment AST node to a command's assignment list
    Returns 1 on success, 0 on failure
*/
int ast_assignment_append(struct ast *cmd, struct ast *assignment)
{
    if (!cmd || cmd->type != AST_CMD || !assignment
        || assignment->type != AST_ASSIGNMENT)
    {
        return 0;
    }

    struct ast **head = &cmd->data.ast_cmd.assignments;
    if (!*head)
    {
        *head = assignment;
        return 1;
    }

    struct ast *cur = *head;
    while (cur->data.ast_assignment.next)
    {
        cur = cur->data.ast_assignment.next;
    }
    cur->data.ast_assignment.next = assignment;
    return 1;
}