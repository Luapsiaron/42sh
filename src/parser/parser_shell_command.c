#include "parser.h"
#include "parser_internal.h"

static const enum token_type END_TOKENS_BRACE[] = { TOKEN_RBRACE };

struct ast *parse_block(struct parser *p)
{
    if (peek(p) != TOKEN_LBRACE)
    {
        return NULL;
    }

    pop(p);
    skip_newlines(p);

    struct ast *body = parse_compound_list(p, END_TOKENS_BRACE,
                                           sizeof(END_TOKENS_BRACE)
                                               / sizeof(*END_TOKENS_BRACE));
    if (!body)
    {
        return NULL;
    }

    if (peek(p) != TOKEN_RBRACE)
    {
        ast_free(body);
        return NULL;
    }
    pop(p);
    return ast_block_init(body);
}

struct ast *parse_shell_command(struct parser *p)
{
    if (peek(p) == TOKEN_LBRACE)
    {
        return parse_block(p);
    }
    if (peek(p) == TOKEN_IF)
    {
        return parse_if(p);
    }
    if (peek(p) == TOKEN_FOR)
    {
        return parse_for(p);
    }
    if (peek(p) == TOKEN_WHILE)
    {
        return parse_while(p);
    }
    if (peek(p) == TOKEN_UNTIL)
    {
        return parse_until(p);
    }
    return NULL;
}

static void redir_list_append(struct ast **head, struct ast *node)
{
    if (!head || !node)
    {
        return;
    }
    if (!*head)
    {
        *head = node;
        return;
    }
    struct ast *cur = *head;
    while (cur->data.ast_redir.next)
    {
        cur = cur->data.ast_redir.next;
    }
    cur->data.ast_redir.next = node;
}

static char *parse_funcdec_header(struct parser *p)
{
    if (peek(p) != TOKEN_WORD || peek_next(p) != TOKEN_LPAREN)
    {
        return NULL;
    }

    const char *lex = p->current_token->lexeme;
    if (!lex)
    {
        return NULL;
    }

    char *fname = xstrdup(lex);
    if (!fname)
    {
        return NULL;
    }
    pop(p); // WORD

    if (peek(p) != TOKEN_LPAREN)
    {
        free(fname);
        return NULL;
    }
    pop(p); // LPAREN

    if (peek(p) != TOKEN_RPAREN)
    {
        free(fname);
        return NULL;
    }
    pop(p); // RPAREN

    return fname;
}

struct ast *parse_redir_list(struct parser *p)
{
    struct ast *redirs = NULL;
    while (peek(p) == TOKEN_IONUMBER || is_redirection_token(peek(p)))
    {
        struct ast *redir = parse_redirection(p);
        if (!redir)
        {
            ast_free(redirs);
            return NULL;
        }
        redir_list_append(&redirs, redir);
    }
    return redirs;
}

struct ast *parse_funcdec(struct parser *p)
{
    char *fname = parse_funcdec_header(p);
    if (!fname)
    {
        return NULL;
    }

    skip_newlines(p);

    struct ast *body = parse_shell_command(p);
    if (!body)
    {
        free(fname);
        return NULL;
    }

    struct ast *redirs = parse_redir_list(p);
    if (parse_error_occurred())
    {
        free(fname);
        ast_free(redirs);
        return NULL;
    }

    struct ast *funcdec = ast_funcdec_init(fname, body, redirs);
    free(fname);
    if (!funcdec)
    {
        ast_free(body);
        ast_free(redirs);
        return NULL;
    }
    return funcdec;
}
