#ifndef PARSER_INTERNAL_H
#define PARSER_INTERNAL_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../lexer/token/token.h"

struct parser
{
    struct lexer lexer;
    struct token *current_token;
};

enum token_type peek(struct parser *parser);
void pop(struct parser *parser);

int remove_separator(struct parser *p);
void skip_newlines(struct parser *p);
void skip_semicolon_newline(struct parser *p);

struct ast *parse_simple_command(struct parser *p);
struct ast *parse_list(struct parser *p);
struct ast *parse_command(struct parser *p);

struct ast *parse_compound_list(struct parser *p, const enum token_type *end_token,
                           size_t end_token_count);

struct ast *parse_if(struct parser *p);
struct ast *parse_while(struct parser *p);
struct ast *parse_until(struct parser *p);
struct ast *parse_for(struct parser *p);

struct ast *parse_pipeline(struct parser *p);

struct ast *parse_and_or(struct parser *p);

struct ast *parse_redirection(struct parser *p);
int is_redirection_token(enum token_type type);

struct ast *parse_assignment(struct parser *p);

#endif /* ! PARSER_INTERNAL_H */
