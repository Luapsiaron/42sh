#define _POSIX_C_SOURCE 200809L
#include <criterion/criterion.h>
#include <criterion/redirect.h>
#include <string.h>

#include "../../src/parser/parser.h"
#include "../../src/utils/parser/pretty_printer.h"

static FILE *fmem_from_str(const char *s)
{
    return fmemopen((void *)s, strlen(s), "r");
}

static void redirect_all_stdout(void)
{
    cr_redirect_stdout();
}

Test(ast_printer, simple_command, .init = redirect_all_stdout)
{
    FILE *f = fmem_from_str("echo cmd");
    struct ast *ast = parse_input(f);

    cr_assert_not_null(ast);
    ast_pretty_print(ast, stdout);
    fflush(stdout);
    cr_assert_stdout_eq_str("command \"echo\" \"cmd\"\n");

    ast_free(ast);
}

Test(ast_printer, if_elif_command, .init = redirect_all_stdout)
{
    FILE *f = fmem_from_str(
        "if echo cmd; then false; elif echo oui; then test; else true; fi");
    struct ast *ast = parse_input(f);

    cr_assert_not_null(ast);
    ast_pretty_print(ast, stdout);
    fflush(stdout);
    cr_assert_stdout_eq_str(
        "if { command \"echo\" \"cmd\" }; then { command \"false\" }; elif { "
        "command \"echo\" \"oui\" }; then { command \"test\" }; else { command "
        "\"true\" }; fi\n");

    ast_free(ast);
}

Test(ast_printer, pipe, .init = redirect_all_stdout)
{
    FILE *f = fmem_from_str("echo oui | echo non || echo abc");
    struct ast *ast = parse_input(f);

    cr_assert_not_null(ast);
    ast_pretty_print(ast, stdout);
    fflush(stdout);
    cr_assert_stdout_eq_str(
        "and_or { pipeline { command \"echo\" \"oui\" | command \"echo\" "
        "\"non\" } || command \"echo\" \"abc\" }\n");

    ast_free(ast);
}

Test(ast_printer, and, .init = redirect_all_stdout)
{
    FILE *f = fmem_from_str("echo oui & echo non && echo abc");
    struct ast *ast = parse_input(f);

    cr_assert_not_null(ast);
    ast_pretty_print(ast, stdout);
    fflush(stdout);
    cr_assert_stdout_eq_str("and_or { command \"echo\" \"oui\" \"&\" \"echo\" "
                            "\"non\" && command \"echo\" \"abc\" }\n");

    ast_free(ast);
}

Test(ast_printer, while_loop, .init = redirect_all_stdout)
{
    FILE *f = fmem_from_str("while true; do echo INFINIT; done");
    struct ast *ast = parse_input(f);

    cr_assert_not_null(ast);
    ast_pretty_print(ast, stdout);
    fflush(stdout);
    cr_assert_stdout_eq_str("while { command \"true\" }; do { command \"echo\" "
                            "\"INFINIT\" }; done \n");

    ast_free(ast);
}

Test(ast_printer, until_loop, .init = redirect_all_stdout)
{
    FILE *f = fmem_from_str("until true; do echo INFINIT; done");
    struct ast *ast = parse_input(f);

    cr_assert_not_null(ast);
    ast_pretty_print(ast, stdout);
    fflush(stdout);
    cr_assert_stdout_eq_str("until { command \"true\" }; do { command \"echo\" "
                            "\"INFINIT\" }; done \n");

    ast_free(ast);
}

Test(ast_printer, redirection, .init = redirect_all_stdout)
{
    FILE *f = fmem_from_str("echo oui > file; echo non >> file2; echo 1 >| "
                            "non; 1 <& 2; 1 >& 2; un <> deux;");
    struct ast *ast = parse_input(f);

    cr_assert_not_null(ast);
    ast_pretty_print(ast, stdout);
    fflush(stdout);
    cr_assert_stdout_eq_str(
        "command \"echo\" \"oui\" 1>  \"file\"; command \"echo\" \"non\" 1>>  "
        "\"file2\"; command \"echo\" \"1\" 1>|  \"non\"; command \"1\" 0<&  "
        "\"2\"; command \"1\" 1>&  \"2\"; command \"un\" 0<>  \"deux\"\n");

    ast_free(ast);
}

Test(ast_printer, for_loop, .init = redirect_all_stdout)
{
    FILE *f = fmem_from_str("for true; do echo INFINIT; done; for true in "
                            "false; do echo INFINIT; done");
    struct ast *ast = parse_input(f);

    cr_assert_not_null(ast);
    ast_pretty_print(ast, stdout);
    fflush(stdout);
    cr_assert_stdout_eq_str(
        "for command \"true\"; do { command \"echo\" \"INFINIT\" }; done ; for "
        "command \"true\" in command \"false\"; do { command \"echo\" "
        "\"INFINIT\" }; done \n");

    ast_free(ast);
}

Test(ast_printer, negation, .init = redirect_all_stdout)
{
    FILE *f = fmem_from_str("! echo command");
    struct ast *ast = parse_input(f);

    cr_assert_not_null(ast);
    ast_pretty_print(ast, stdout);
    fflush(stdout);
    cr_assert_stdout_eq_str("negation command \"echo\" \"command\"\n");

    ast_free(ast);
}

Test(ast_printer, func, .init = redirect_all_stdout)
{
    FILE *f = fmem_from_str(
        "my_func() { echo Arg1: $1; echo Arg2: $2; }; my_func first second");
    struct ast *ast = parse_input(f);

    cr_assert_not_null(ast);
    ast_pretty_print(ast, stdout);
    fflush(stdout);
    cr_assert_stdout_eq_str(
        "funcdec \"my_func\" () block { command \"echo\" \"Arg1:\" \"$1\"; "
        "command \"echo\" \"Arg2:\" \"$2\" }; command \"my_func\" \"first\" "
        "\"second\"\n");

    ast_free(ast);
}
