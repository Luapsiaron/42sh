#include "condition.h"

#include "execution.h"
#include "../ast/ast.h"


int eval_condition(ast_t *condition)
{
    if (condition->data.ast_and_or.operator == AND_OP) 
    {
        return (exec_ast(condition->data.ast_and_or.left) &&
                exec_ast(condition->data.ast_and_or.right));
    }
    return (exec_ast(condition->data.ast_and_or.left) ||
            exec_ast(condition->data.ast_and_or.right)); 
}