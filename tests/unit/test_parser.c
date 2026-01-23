#define _POSIX_C_SOURCE 200809L
#include <criterion/criterion.h>
#include <string.h>

#include "../../src/parser/parser.h"
#include "../../src/parser/parser_internal.h"

static FILE *fmem_from_str(const char *s)
{
    return fmemopen((void *)s, strlen(s), "r");
}

Test(parser, simple_command)
{
    FILE *f = fmem_from_str("true\n");
    struct ast *ast = parse_input(f);
    fclose(f);

    cr_assert_not_null(ast);
    cr_assert_eq(ast->type, AST_LIST);
    cr_assert_str_eq(ast->data.ast_list.child->data.ast_cmd.argv[0], "true");

    ast_free(ast);
}

Test(parser, command_list_two_cmds)
{
    FILE *f = fmem_from_str("true; false\n");
    struct ast *ast = parse_input(f);
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

Test(parse_if, simple_if)
{
    FILE *f = fmem_from_str("if true; then false\n fi");
    struct parser p;
    p.current_token = NULL;
    p.next_token = NULL;

    lexer_init(&p.lexer, f);
    p.current_token = lexer_next(&p.lexer);

    cr_assert_not_null(p.current_token);

    struct ast *ast = parse_if(&p);
    fclose(f);

    cr_assert_not_null(ast);

    cr_assert_not_null(ast->data.ast_if.condition);
    cr_assert_not_null(ast->data.ast_if.then_body);

    cr_assert_not_null(ast->data.ast_if.condition->data.ast_list.child);
    cr_assert_not_null(ast->data.ast_if.then_body->data.ast_list.child);

    cr_assert_not_null(
        ast->data.ast_if.condition->data.ast_list.child->data.ast_cmd.argv);
    cr_assert_not_null(
        ast->data.ast_if.then_body->data.ast_list.child->data.ast_cmd.argv);

    cr_assert_str_eq(
        ast->data.ast_if.condition->data.ast_list.child->data.ast_cmd.argv[0],
        "true");
    cr_assert_str_eq(
        ast->data.ast_if.then_body->data.ast_list.child->data.ast_cmd.argv[0],
        "false");

    ast_free(ast);
}

Test(parse_if, simple_if_else)
{
    FILE *f = fmem_from_str("if true; then false\n else true; fi");
    struct parser p;
    p.current_token = NULL;
    p.next_token = NULL;

    lexer_init(&p.lexer, f);
    p.current_token = lexer_next(&p.lexer);
    if (!p.current_token)
    {
        return;
    }

    struct ast *ast = parse_if(&p);
    fclose(f);

    cr_assert_not_null(ast);

    cr_assert_not_null(ast->data.ast_if.condition);
    cr_assert_not_null(ast->data.ast_if.then_body);
    cr_assert_not_null(ast->data.ast_if.else_body);

    cr_assert_not_null(ast->data.ast_if.condition->data.ast_list.child);
    cr_assert_not_null(ast->data.ast_if.then_body->data.ast_list.child);
    cr_assert_not_null(ast->data.ast_if.else_body->data.ast_list.child);

    cr_assert_not_null(
        ast->data.ast_if.condition->data.ast_list.child->data.ast_cmd.argv);
    cr_assert_not_null(
        ast->data.ast_if.then_body->data.ast_list.child->data.ast_cmd.argv);
    cr_assert_not_null(
        ast->data.ast_if.else_body->data.ast_list.child->data.ast_cmd.argv);

    cr_assert_str_eq(
        ast->data.ast_if.condition->data.ast_list.child->data.ast_cmd.argv[0],
        "true");
    cr_assert_str_eq(
        ast->data.ast_if.then_body->data.ast_list.child->data.ast_cmd.argv[0],
        "false");
    cr_assert_str_eq(
        ast->data.ast_if.else_body->data.ast_list.child->data.ast_cmd.argv[0],
        "true");

    ast_free(ast);
}

Test(parse_if, simple_if_elif_else)
{
    FILE *f = fmem_from_str(
        "if true; then false\n elif true; then false; else true; fi");
    struct parser p;
    p.current_token = NULL;
    p.next_token = NULL;

    lexer_init(&p.lexer, f);
    p.current_token = lexer_next(&p.lexer);
    if (!p.current_token)
    {
        return;
    }

    struct ast *ast = parse_if(&p);
    fclose(f);

    cr_assert_not_null(ast);

    cr_assert_not_null(ast->data.ast_if.condition);
    cr_assert_not_null(ast->data.ast_if.then_body);
    cr_assert_not_null(ast->data.ast_if.else_body);

    cr_assert_not_null(ast->data.ast_if.condition->data.ast_list.child);
    cr_assert_not_null(ast->data.ast_if.then_body->data.ast_list.child);
    cr_assert_not_null(ast->data.ast_if.else_body->data.ast_list.child);

    cr_assert_not_null(
        ast->data.ast_if.condition->data.ast_list.child->data.ast_cmd.argv);
    cr_assert_not_null(
        ast->data.ast_if.then_body->data.ast_list.child->data.ast_cmd.argv);

    cr_assert_not_null(ast->data.ast_if.else_body);

    struct ast *v_else = ast->data.ast_if.else_body;

    cr_assert_not_null(v_else->data.ast_if.condition);
    cr_assert_not_null(v_else->data.ast_if.then_body);
    cr_assert_not_null(v_else->data.ast_if.else_body);

    cr_assert_not_null(v_else->data.ast_if.condition->data.ast_list.child);
    cr_assert_not_null(v_else->data.ast_if.then_body->data.ast_list.child);
    cr_assert_not_null(v_else->data.ast_if.else_body->data.ast_list.child);

    cr_assert_not_null(
        v_else->data.ast_if.condition->data.ast_list.child->data.ast_cmd.argv);
    cr_assert_not_null(
        v_else->data.ast_if.then_body->data.ast_list.child->data.ast_cmd.argv);
    cr_assert_not_null(
        v_else->data.ast_if.else_body->data.ast_list.child->data.ast_cmd.argv);

    cr_assert_str_eq(
        ast->data.ast_if.condition->data.ast_list.child->data.ast_cmd.argv[0],
        "true");
    cr_assert_str_eq(
        ast->data.ast_if.then_body->data.ast_list.child->data.ast_cmd.argv[0],
        "false");

    cr_assert_str_eq(v_else->data.ast_if.condition->data.ast_list.child->data
                         .ast_cmd.argv[0],
                     "true");
    cr_assert_str_eq(v_else->data.ast_if.then_body->data.ast_list.child->data
                         .ast_cmd.argv[0],
                     "false");
    cr_assert_str_eq(v_else->data.ast_if.else_body->data.ast_list.child->data
                         .ast_cmd.argv[0],
                     "true");

    ast_free(ast);
}

Test(parse_if, simple_if_multiple_condition)
{
    FILE *f = fmem_from_str("if true; false; echo; then false\n fi");
    struct parser p;
    p.current_token = NULL;
    p.next_token = NULL;

    lexer_init(&p.lexer, f);
    p.current_token = lexer_next(&p.lexer);
    if (!p.current_token)
    {
        return;
    }

    struct ast *ast = parse_if(&p);
    fclose(f);

    cr_assert_not_null(ast);

    cr_assert_not_null(ast->data.ast_if.condition);
    cr_assert_not_null(ast->data.ast_if.then_body);

    cr_assert_not_null(ast->data.ast_if.condition->data.ast_list.child);
    cr_assert_not_null(ast->data.ast_if.condition->data.ast_list.next);
    cr_assert_not_null(ast->data.ast_if.then_body->data.ast_list.child);
    cr_assert_not_null(
        ast->data.ast_if.then_body->data.ast_list.child->data.ast_cmd.argv);

    cr_assert_not_null(
        ast->data.ast_if.condition->data.ast_list.next->data.ast_list.child);
    cr_assert_not_null(ast->data.ast_if.condition->data.ast_list.next->data
                           .ast_list.child->data.ast_cmd.argv);
    cr_assert_not_null(
        ast->data.ast_if.condition->data.ast_list.next->data.ast_list.next);

    cr_assert_not_null(ast->data.ast_if.condition->data.ast_list.next->data
                           .ast_list.next->data.ast_list.child);
    cr_assert_not_null(
        ast->data.ast_if.condition->data.ast_list.next->data.ast_list.next->data
            .ast_list.child->data.ast_cmd.argv);

    cr_assert_str_eq(
        ast->data.ast_if.condition->data.ast_list.child->data.ast_cmd.argv[0],
        "true");
    cr_assert_str_eq(ast->data.ast_if.condition->data.ast_list.next->data
                         .ast_list.child->data.ast_cmd.argv[0],
                     "false");
    cr_assert_str_eq(
        ast->data.ast_if.condition->data.ast_list.next->data.ast_list.next->data
            .ast_list.child->data.ast_cmd.argv[0],
        "echo");

    cr_assert_str_eq(
        ast->data.ast_if.then_body->data.ast_list.child->data.ast_cmd.argv[0],
        "false");

    ast_free(ast);
}

Test(parse_for, simple_for)
{
    FILE *f = fmem_from_str("for condition; do echo test\n echo pls; done");
    struct parser p;
    p.current_token = NULL;
    p.next_token = NULL;

    lexer_init(&p.lexer, f);
    p.current_token = lexer_next(&p.lexer);
    if (!p.current_token)
    {
        return;
    }

    struct ast *ast = parse_for(&p);
    fclose(f);

    cr_assert_not_null(ast);

    cr_assert_not_null(ast->data.ast_for.first_arg);
    cr_assert_not_null(ast->data.ast_for.first_arg->data.ast_cmd.argv);
    cr_assert_str_eq(ast->data.ast_for.first_arg->data.ast_cmd.argv[0],
                     "condition");

    cr_assert_not_null(ast->data.ast_for.body);
    cr_assert_not_null(ast->data.ast_for.body->data.ast_list.child);
    cr_assert_not_null(ast->data.ast_for.body->data.ast_list.next);
    cr_assert_not_null(
        ast->data.ast_for.body->data.ast_list.child->data.ast_cmd.argv);
    cr_assert_str_eq(
        ast->data.ast_for.body->data.ast_list.child->data.ast_cmd.argv[0],
        "echo");
    cr_assert_str_eq(
        ast->data.ast_for.body->data.ast_list.child->data.ast_cmd.argv[1],
        "test");

    cr_assert_not_null(
        ast->data.ast_for.body->data.ast_list.next->data.ast_list.child);
    cr_assert_not_null(ast->data.ast_for.body->data.ast_list.next->data.ast_list
                           .child->data.ast_cmd.argv);
    cr_assert_str_eq(ast->data.ast_for.body->data.ast_list.next->data.ast_list
                         .child->data.ast_cmd.argv[0],
                     "echo");
    cr_assert_str_eq(ast->data.ast_for.body->data.ast_list.next->data.ast_list
                         .child->data.ast_cmd.argv[1],
                     "pls");

    ast_free(ast);
}

Test(parse_for, simple_for_in)
{
    FILE *f =
        fmem_from_str("for var in sec_var; do echo test\n echo pls; done");
    struct parser p;
    p.current_token = NULL;
    p.next_token = NULL;

    lexer_init(&p.lexer, f);
    p.current_token = lexer_next(&p.lexer);
    if (!p.current_token)
    {
        return;
    }

    struct ast *ast = parse_for(&p);
    fclose(f);

    cr_assert_not_null(ast);

    cr_assert_not_null(ast->data.ast_for.first_arg);
    cr_assert_not_null(ast->data.ast_for.first_arg->data.ast_cmd.argv);
    cr_assert_str_eq(ast->data.ast_for.first_arg->data.ast_cmd.argv[0], "var");

    cr_assert_not_null(ast->data.ast_for.second_arg);
    cr_assert_not_null(ast->data.ast_for.second_arg->data.ast_cmd.argv);
    cr_assert_str_eq(ast->data.ast_for.second_arg->data.ast_cmd.argv[0],
                     "sec_var");

    cr_assert_not_null(ast->data.ast_for.body);
    cr_assert_not_null(ast->data.ast_for.body->data.ast_list.child);
    cr_assert_not_null(ast->data.ast_for.body->data.ast_list.next);
    cr_assert_not_null(
        ast->data.ast_for.body->data.ast_list.child->data.ast_cmd.argv);
    cr_assert_str_eq(
        ast->data.ast_for.body->data.ast_list.child->data.ast_cmd.argv[0],
        "echo");
    cr_assert_str_eq(
        ast->data.ast_for.body->data.ast_list.child->data.ast_cmd.argv[1],
        "test");

    cr_assert_not_null(
        ast->data.ast_for.body->data.ast_list.next->data.ast_list.child);
    cr_assert_not_null(ast->data.ast_for.body->data.ast_list.next->data.ast_list
                           .child->data.ast_cmd.argv);
    cr_assert_str_eq(ast->data.ast_for.body->data.ast_list.next->data.ast_list
                         .child->data.ast_cmd.argv[0],
                     "echo");
    cr_assert_str_eq(ast->data.ast_for.body->data.ast_list.next->data.ast_list
                         .child->data.ast_cmd.argv[1],
                     "pls");

    ast_free(ast);
}

Test(parse_while, simple_while)
{
    FILE *f = fmem_from_str("while condition; do echo test\n echo pls; done");
    struct parser p;
    p.current_token = NULL;
    p.next_token = NULL;

    lexer_init(&p.lexer, f);
    p.current_token = lexer_next(&p.lexer);
    if (!p.current_token)
    {
        return;
    }

    struct ast *ast = parse_while(&p);
    fclose(f);

    cr_assert_not_null(ast);

    cr_assert_eq(ast->data.ast_while_until.type, LOOP_WHILE);

    cr_assert_not_null(ast->data.ast_while_until.condition);
    cr_assert_not_null(
        ast->data.ast_while_until.condition->data.ast_list.child);
    cr_assert_not_null(ast->data.ast_while_until.condition->data.ast_list.child
                           ->data.ast_cmd.argv);
    cr_assert_str_eq(ast->data.ast_while_until.condition->data.ast_list.child
                         ->data.ast_cmd.argv[0],
                     "condition");

    cr_assert_not_null(ast->data.ast_while_until.body);
    cr_assert_not_null(ast->data.ast_while_until.body->data.ast_list.child);
    cr_assert_not_null(
        ast->data.ast_while_until.body->data.ast_list.child->data.ast_cmd.argv);
    cr_assert_str_eq(ast->data.ast_while_until.body->data.ast_list.child->data
                         .ast_cmd.argv[0],
                     "echo");
    cr_assert_str_eq(ast->data.ast_while_until.body->data.ast_list.child->data
                         .ast_cmd.argv[1],
                     "test");

    cr_assert_not_null(ast->data.ast_while_until.body->data.ast_list.next);
    cr_assert_not_null(
        ast->data.ast_while_until.body->data.ast_list.child->data.ast_cmd.argv);
    cr_assert_not_null(ast->data.ast_while_until.body->data.ast_list.next->data
                           .ast_list.child->data.ast_cmd.argv);
    cr_assert_str_eq(ast->data.ast_while_until.body->data.ast_list.next->data
                         .ast_list.child->data.ast_cmd.argv[0],
                     "echo");
    cr_assert_str_eq(ast->data.ast_while_until.body->data.ast_list.next->data
                         .ast_list.child->data.ast_cmd.argv[1],
                     "pls");

    ast_free(ast);
}

Test(parse_until, simple_until)
{
    FILE *f = fmem_from_str("until condition; do echo test\n echo pls; done");
    struct parser p;
    p.current_token = NULL;
    p.next_token = NULL;

    lexer_init(&p.lexer, f);
    p.current_token = lexer_next(&p.lexer);
    if (!p.current_token)
    {
        return;
    }

    struct ast *ast = parse_until(&p);
    fclose(f);

    cr_assert_not_null(ast);

    cr_assert_eq(ast->data.ast_while_until.type, LOOP_UNTIL);

    cr_assert_not_null(ast->data.ast_while_until.condition);
    cr_assert_not_null(
        ast->data.ast_while_until.condition->data.ast_list.child);
    cr_assert_not_null(ast->data.ast_while_until.condition->data.ast_list.child
                           ->data.ast_cmd.argv);
    cr_assert_str_eq(ast->data.ast_while_until.condition->data.ast_list.child
                         ->data.ast_cmd.argv[0],
                     "condition");

    cr_assert_not_null(ast->data.ast_while_until.body);
    cr_assert_not_null(ast->data.ast_while_until.body->data.ast_list.child);
    cr_assert_not_null(
        ast->data.ast_while_until.body->data.ast_list.child->data.ast_cmd.argv);
    cr_assert_str_eq(ast->data.ast_while_until.body->data.ast_list.child->data
                         .ast_cmd.argv[0],
                     "echo");
    cr_assert_str_eq(ast->data.ast_while_until.body->data.ast_list.child->data
                         .ast_cmd.argv[1],
                     "test");

    cr_assert_not_null(ast->data.ast_while_until.body->data.ast_list.next);
    cr_assert_not_null(
        ast->data.ast_while_until.body->data.ast_list.child->data.ast_cmd.argv);
    cr_assert_not_null(ast->data.ast_while_until.body->data.ast_list.next->data
                           .ast_list.child->data.ast_cmd.argv);
    cr_assert_str_eq(ast->data.ast_while_until.body->data.ast_list.next->data
                         .ast_list.child->data.ast_cmd.argv[0],
                     "echo");
    cr_assert_str_eq(ast->data.ast_while_until.body->data.ast_list.next->data
                         .ast_list.child->data.ast_cmd.argv[1],
                     "pls");

    ast_free(ast);
}
