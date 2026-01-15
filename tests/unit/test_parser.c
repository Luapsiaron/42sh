#define _POSIX_C_SOURCE 200809L
#include <criterion/criterion.h>
#include <string.h>

#include "../../src/ast/ast.h"
#include "../../src/parser/parser.h"

static FILE *fmem_from_str(const char *s)
{
    return fmemopen((void *)s, strlen(s), "r");
}

Test(parser, simple_command)
{
    FILE *f = fmem_from_str("true\n");
    ast_t *ast = parse_input(f);
    fclose(f);

    cr_assert_not_null(ast);
    cr_assert_eq(ast->type, AST_LIST);
    cr_assert_str_eq(ast->data.ast_list.child->data.ast_cmd.argv[0], "true");

    ast_free(ast);
}

Test(parser, command_list_two_cmds)
{
    FILE *f = fmem_from_str("true; false\n");
    ast_t *ast = parse_input(f);
    fclose(f);

    cr_assert_not_null(ast);
    cr_assert_eq(ast->type, AST_LIST);

    cr_assert_not_null(ast->data.ast_list.child);
    cr_assert_eq(ast->data.ast_list.child->type, AST_CMD);
    cr_assert_str_eq(ast->data.ast_list.child->data.ast_cmd.argv[0], "true");

    cr_assert_not_null(ast->data.ast_list.next);
    cr_assert_eq(ast->data.ast_list.next->type, AST_LIST);
    cr_assert_eq(ast->data.ast_list.next->data.ast_list.child->type, AST_CMD);
    cr_assert_str_eq(
        ast->data.ast_list.next->data.ast_list.child->data.ast_cmd.argv[0],
        "false");

    ast_free(ast);
}
