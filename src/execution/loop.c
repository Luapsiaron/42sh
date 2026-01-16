#include "execution/loop.h"
#include "execution.h"
#include "../ast/ast.h"
#include "../expansion/hashmap.h"


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
    // TODO
    (void)ast;
    (void)hm;
    return 0;
}
