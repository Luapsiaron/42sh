#include "condition.h"

#include "../ast/ast.h"
#include "../expansion/hashmap.h"
#include "execution.h"

int eval_condition(struct ast *condition, struct hash_map *hm) // evaluates a condition AST node
{
    if (condition->data.ast_and_or.operator== AND_OP)
    {
        return (exec_ast(condition->data.ast_and_or.left, hm)
                && exec_ast(condition->data.ast_and_or.right, hm));
    }
    return (exec_ast(condition->data.ast_and_or.left, hm)
            || exec_ast(condition->data.ast_and_or.right, hm));
}