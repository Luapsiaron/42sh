#include "condition.h"

#include "execution.h"
#include "../ast/ast.h"
#include "../expansion/hashmap.h"


int eval_condition(ast_t *condition, struct hash_map *hm)
{
    if (condition->data.ast_and_or.operator == AND_OP) 
    {
        return (exec_ast(condition->data.ast_and_or.left, hm) &&
                exec_ast(condition->data.ast_and_or.right, hm));
    }
    return (exec_ast(condition->data.ast_and_or.left, hm) ||
            exec_ast(condition->data.ast_and_or.right, hm)); 
}