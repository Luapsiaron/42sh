#include "execution/loop.h"
#include "execution.h"
#include "../ast/ast.h"

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

int exec_while_until(ast_t *ast)
{
    if (!ast)
        return 0;
    struct ast_while_until *wu = &ast->data.ast_while_until;
    int last_status = 0;
    while (1)
    {
        int cond_status = exec_ast(wu->condition);
        if (!loop_should_continue(wu->type, cond_status))
            break;
        last_status = exec_ast(wu->body);
    }
    return last_status;
}




int exec_for(ast_t *ast)
{
    // TODO
    (void)ast;
    return 0;
}
