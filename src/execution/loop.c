#include "loop.h"

#include "../ast/ast.h"
#include "../builtins/break.h"
#include "../expansion/expand.h"
#include "../expansion/hashmap.h"
#include "execution.h"

static int
loop_should_continue(enum loop_type type,
                     int cond_status) // determines if loop should continue
{
    if (type == LOOP_WHILE)
        return cond_status == 0;
    return cond_status != 0;
}

static int depth_get(struct hash_map *hm) // gets current loop depth
{
    char *v = hash_map_get(hm, "loop_depth");
    return v ? atoi(v) : 0;
}

static void depth_set(struct hash_map *hm, int d) // sets current loop depth
{
    char buf[32];
    snprintf(buf, sizeof(buf), "%d", d);
    hash_map_insert(hm, "loop_depth", buf, NULL);
}

static void depth_enter(struct hash_map *hm) // increments loop depth
{
    depth_set(hm, depth_get(hm) + 1);
}

static void depth_leave(struct hash_map *hm) // decrements loop depth
{
    int d = depth_get(hm);
    if (d > 0)
        depth_set(hm, d - 1);
}

static int break_level(int st) // determines break level from status code
{
    if (st < RET_BREAK_BASE || st >= RET_BREAK_BASE + RET_BREAK_MAX)
        return -1;
    return st - RET_BREAK_BASE + 1;
}

static int break_propagate(int lvl) // propagates break level
{
    if (lvl <= 1)
        return 0;
    return RET_BREAK_BASE + (lvl - 2);
}

int exec_while_until(
    struct ast *ast,
    struct hash_map *hm) // executes a while/until loop AST node
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
        depth_enter(hm);
        last_status = exec_ast(wu->body, hm);
        depth_leave(hm);
        int lvl = break_level(last_status);
        if (lvl != -1)
            return break_propagate(lvl);
    }
    return last_status;
}

int exec_for(struct ast *ast,
             struct hash_map *hm) // executes a for loop AST node
{
    if (!ast || ast->type != AST_FOR)
        return 2;
    struct ast_for *f = &ast->data.ast_for;
    if (!f->first_arg || f->first_arg->type != AST_CMD
        || !f->first_arg->data.ast_cmd.argv
        || !f->first_arg->data.ast_cmd.argv[0])
        return 2;
    const char *var_name = f->first_arg->data.ast_cmd.argv[0];
    if (!f->second_arg || f->second_arg->type != AST_CMD
        || !f->second_arg->data.ast_cmd.argv)
        return 0;
    char **values = expand_argv(f->second_arg->data.ast_cmd.argv, hm);
    if (!values)
        return 2;
    int last_status = 0;
    for (size_t i = 0; values[i]; i++)
    {
        hash_map_insert(hm, var_name, values[i], NULL);

        depth_enter(hm);
        last_status = exec_ast(f->body, hm);
        depth_leave(hm);

        int lvl = break_level(last_status);
        if (lvl != -1)
            return free_argv(values), break_propagate(lvl);
    }
    free_argv(values);
    return last_status;
}
