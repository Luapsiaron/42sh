#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <string.h>

#include "../../src/ast/ast.h"
#include "../../src/ast/ast_free.h"
#include "../../src/execution/execution.h"
#include "../../src/utils/str/str.h"

static void redirect_all_stdout(void)
{
    cr_redirect_stdout();
}

static ast_t *mk_cmd(const char *w)
{
    char **argv = calloc(2, sizeof(char *));
    argv[0] = xstrdup(w);
    return ast_cmd_init(argv);
}

Test(execution, exec_cmd_true_false)
{
    ast_t *t = mk_cmd("true");
    ast_t *f = mk_cmd("false");

    cr_assert_eq(exec_ast(t), 0);
    cr_assert_eq(exec_ast(f), 1);

    ast_free(t);
    ast_free(f);
}

Test(execution, exec_list_returns_last_status)
{
    ast_t *c1 = mk_cmd("true");
    ast_t *c2 = mk_cmd("false");

    ast_t *l2 = ast_list_init(NULL, c2);
    ast_t *l1 = ast_list_init(l2, c1);

    cr_assert_eq(exec_ast(l1), 1);

    ast_free(l1);
}

Test(execution, exec_if_then_vs_else)
{
    ast_t *cond = mk_cmd("false");
    ast_t *thenb = ast_list_init(NULL, mk_cmd("true"));
    ast_t *elseb = ast_list_init(NULL, mk_cmd("false"));

    ast_t *ifn = ast_if_init(cond, thenb, elseb);
    cr_assert_eq(exec_ast(ifn), 1);

    ast_free(ifn);
}

Test(execution, exec_echo_prints, .init = redirect_all_stdout)
{
    char **argv = calloc(3, sizeof(char *));
    argv[0] = xstrdup("echo");
    argv[1] = xstrdup("hello");
    ast_t *cmd = ast_cmd_init(argv);

    cr_assert_eq(exec_ast(cmd), 0);
    cr_assert_stdout_eq_str("hello\n");

    ast_free(cmd);
}
