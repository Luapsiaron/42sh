#include "execution/loop.h"

#include "../ast/ast.h"
#include "../expansion/expand.h"
#include "../expansion/hashmap.h"
#include "execution.h"

static int loop_should_continue(loop_t type, int cond_status)
{
    if (type == LOOP_WHILE)
    {
        if (cond_status == 0)
            return 1;
        return 0;
    }
    if (cond_status != 0)
        return 1;
    return 0;
}

int exec_while_until(ast_t *ast, struct hash_map *hm)
{
    if (!ast)
        return 0;
    struct ast_while_until *wu = &ast->data.ast_while_until;
    int last_status = 0;
    while (1)
    {
        int cond_status = exec_ast(wu->condition, hm);
        if (!loop_should_continue(wu->type, cond_status))
            break;
        last_status = exec_ast(wu->body, hm);
    }
    return last_status;
}

int exec_for(ast_t *ast, struct hash_map *hm)
{
    if (!ast || ast->type != AST_FOR)
        return 2;
    struct ast_for *f = &ast->data.ast_for;
    if (!f->first_arg || f->first_arg->type != AST_CMD
        || !f->first_arg->data.ast_cmd.argv
        || !f->first_arg->data.ast_cmd.argv[0])
        return 2;
    const char *var_name = f->first_arg->data.ast_cmd.argv[0];
    int last_status = 0;
    if (!f->second_arg)
        return 0;

    if (f->second_arg->type != AST_CMD || !f->second_arg->data.ast_cmd.argv)
        return 2;
    char **values = expand_argv(f->second_arg->data.ast_cmd.argv, hm);
    if (!values)
        return 2;
    for (size_t i = 0; values[i]; i++)
    {
        hash_map_insert(hm, var_name, values[i], NULL);
        last_status = exec_ast(f->body, hm);
    }

    return last_status;
}
