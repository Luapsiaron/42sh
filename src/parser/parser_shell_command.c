#include "parser_internal.h"

static const enum token_type END_TOKENS_BRACE[] = {TOKEN_RBRACE};

struct ast *parse_block(struct parser *p)
{
    if(peek(p) != TOKEN_LBRACE)
    {
        return NULL;
    }

    pop(p);
    skip_newlines(p);

    struct ast *body = parse_compound_list(p, END_TOKENS_BRACE, sizeof(END_TOKENS_BRACE) / sizeof(*END_TOKENS_BRACE));
    if(!body)
    {
        return NULL;
    }

    if(peek(p) != TOKEN_RBRACE)
    {
        ast_free(body);
        return NULL;
    }
    pop(p);
    return ast_block_init(body);
}

struct ast *parse_shell_command(struct parser *p)
{
    if(peek(p) == TOKEN_LBRACE)
    {
        return parse_block(p);
    }
    if(peek(p) == TOKEN_IF)
    {
        return parse_if(p);
    }
    if(peek(p) == TOKEN_FOR)
    {
        return parse_for(p);
    }
    if(peek(p) == TOKEN_WHILE)
    {
        return parse_while(p);
    }
    if(peek(p) == TOKEN_UNTIL)
    {
        return parse_until(p);
    }
    return NULL;
}

struct ast *parse_funcdec(struct parser *p)
{
    if(peek(p) != TOKEN_WORD || peek_next(p) != TOKEN_LPAREN)
    {
        return NULL;
    }
    
    const char *lex = p->current_token->lexeme;
    if(!lex)
    {
        return NULL;
    }

    char *fname = xstrdup(lex);
    if(!fname)
    {
        return NULL;
    }
    pop(p); // WORD

    if(peek(p) != TOKEN_LPAREN)
    {
        goto error;
    }
    pop(p); // LPAREN

    if(peek(p) != TOKEN_RPAREN)
    {
        goto error;
    }
    pop(p); // RPAREN

    skip_newlines(p);

    struct ast *body = parse_shell_command(p);
    if(!body)
    {
        goto error;
    }

    struct ast *redirs = NULL;
    while(peek(p) == TOKEN_IONUMBER || is_redirection_token(peek(p)))
    {
        struct ast *redir = parse_redirection(p);
        if(!redir)
        {
            ast_free(redirs);
            ast_free(body);
            goto error;
        }
        if(!redirs)
        {
            redirs = redir;
        }
        else
        {
            struct ast *cur = redirs;
            while(cur->data.ast_redir.next)
            {
                cur = cur->data.ast_redir.next;
            }
            cur->data.ast_redir.next = redir;
        }
    }
    struct ast *func = ast_funcdec_init(fname, body, redirs);
    free(fname);
    if(!func)
    {
        ast_free(body);
        ast_free(redirs);
        return NULL;
    }
    return func;

error:
    free(fname);
    return NULL;
}

struct ast *parse_redir_list(struct parser *p)
{
    struct ast *head = NULL;
    struct ast *tail = NULL;

    while(peek(p) == TOKEN_IONUMBER || is_redirection_token(peek(p)))
    {
        struct ast *redir = parse_redirection(p);
        if(!redir)
        {
            ast_free(head);
            return NULL;
        }
        if(!head)
        {
            head = redir;
        }
        else
        {
            tail->data.ast_redir.next = redir;
        }
        tail = redir;
    }
    return head;
}