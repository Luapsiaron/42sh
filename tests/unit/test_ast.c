#include <criterion/criterion.h>
#include <criterion/redirect.h>

#include "../../src/ast/ast.h"
#include "../../src/ast/ast_free.h"
#include "../../src/utils/str/str.h"

Test(ast, cmd_init_sets_type_and_argv)
{
    char **argv = calloc(2, sizeof(char *));
    cr_assert_not_null(argv);
    argv[0] = xstrdup("true");
    cr_assert_not_null(argv[0]);

    ast_t *cmd = ast_cmd_init(argv);
    cr_assert_not_null(cmd);
    cr_assert_eq(cmd->type, AST_CMD);
    cr_assert_eq(cmd->data.ast_cmd.argv, argv);

    ast_free(cmd);
}

Test(ast, list_and_if_shape)
{
    char **a_true = calloc(2, sizeof(char *));
    a_true[0] = xstrdup("true");
    ast_t *cond = ast_cmd_init(a_true);

    char **a_false = calloc(2, sizeof(char *));
    a_false[0] = xstrdup("false");
    ast_t *then_cmd = ast_cmd_init(a_false);

    char **a_true2 = calloc(2, sizeof(char *));
    a_true2[0] = xstrdup("true");
    ast_t *else_cmd = ast_cmd_init(a_true2);

    ast_t *ifn = ast_if_init(cond, then_cmd, else_cmd);
    cr_assert_not_null(ifn);
    cr_assert_eq(ifn->type, AST_IF);
    cr_assert_eq(ifn->data.ast_if.condition, cond);
    cr_assert_eq(ifn->data.ast_if.then_body, then_cmd);
    cr_assert_eq(ifn->data.ast_if.else_body, else_cmd);

    char **a_true3 = calloc(2, sizeof(char *));
    a_true3[0] = xstrdup("true");
    ast_t *cmd2 = ast_cmd_init(a_true3);

    ast_t *list2 = ast_list_init(NULL, cmd2);
    ast_t *list1 = ast_list_init(list2, ifn);

    cr_assert_eq(list1->type, AST_LIST);
    cr_assert_eq(list1->data.ast_list.child, ifn);
    cr_assert_eq(list1->data.ast_list.next, list2);

    ast_free(list1);
}
