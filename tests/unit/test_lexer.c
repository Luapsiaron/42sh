#define _POSIX_C_SOURCE 200809L
#include <criterion/criterion.h>
#include <string.h>

#include "../../src/lexer/lexer.h"
#include "../../src/lexer/token/token.h"

static FILE *fmem_from_str(const char *s)
{
    return fmemopen((void *)s, strlen(s), "r");
}

static void assert_tok(struct token *t, enum token_type ty, const char *val)
{
    cr_assert_not_null(t);
    cr_assert_eq(t->type, ty);
    if (val)
        cr_assert_str_eq(t->lexeme, val);
    else
        cr_assert_null(t->lexeme);
}

Test(lexer, reserved_words_and_separators)
{
    FILE *f =
        fmem_from_str("if true; then\nfalse\nfi\n for while   until; do done");
    cr_assert_not_null(f);

    struct lexer lx;
    lexer_init(&lx, f);

    struct token *t = NULL;

    t = lexer_next(&lx);
    assert_tok(t, TOKEN_IF, NULL);
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_WORD, "true");
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_SEMICOLON, NULL);
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_THEN, NULL);
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_NEWLINE, NULL);
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_WORD, "false");
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_NEWLINE, NULL);
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_FI, NULL);
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_NEWLINE, NULL);
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_FOR, NULL);
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_WHILE, NULL);
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_UNTIL, NULL);
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_SEMICOLON, NULL);
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_DO, NULL);
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_DONE, NULL);
    token_free(t);
    t = lexer_next(&lx);
    assert_tok(t, TOKEN_EOF, NULL);
    token_free(t);

    fclose(f);
}

Test(lexer, comment_is_skipped)
{
    FILE *f = fmem_from_str("# hello\ntrue\n");
    cr_assert_not_null(f);

    struct lexer lx;
    lexer_init(&lx, f);

    struct token *t = lexer_next(&lx);
    cr_assert_not_null(t);
    cr_assert(t->type == TOKEN_NEWLINE || t->type == TOKEN_WORD);

    token_free(t);
    fclose(f);
}

Test(lexer, negation_token)
{
    FILE *f = fmem_from_str("! true\n");
    cr_assert_not_null(f);

    struct lexer lx;
    lexer_init(&lx, f);

    struct token *t = lexer_next(&lx);
    assert_tok(t, TOKEN_NEGATION, NULL);
    token_free(t);

    t = lexer_next(&lx);
    assert_tok(t, TOKEN_WORD, "true");
    token_free(t);

    fclose(f);
}
