#ifndef PARSER_INTERNAL_H
#define PARSER_INTERNAL_H

#include "../ast/ast.h"
#include "../ast/ast_free.h"
#include "../lexer/lexer.h"
#include "../lexer/token/token.h"

typedef struct parser
{
    lexer_t lexer;
    token_t *current_token;
} parser_t;

token_type_t peek(parser_t *parser);
void pop(parser_t *parser);

int remove_separator(parser_t *p);
void skip_newlines(parser_t *p);
void skip_semicolon_newline(parser_t *p);

ast_t *parse_simple_command(parser_t *p);
ast_t *parse_list(parser_t *p);
ast_t *parse_command(parser_t *p);

ast_t *parse_compound_list(parser_t *p, const token_type_t *end_token,
                           size_t end_token_count);

ast_t *parse_if(parser_t *p);
ast_t *parse_while(parser_t *p);
ast_t *parse_until(parser_t *p);
ast_t *parse_for(parser_t *p);

ast_t *parse_pipeline(parser_t *p);

ast_t *parse_and_or(parser_t *p);

ast_t *parse_redirection(parser_t *p);
int is_redirection_token(token_type_t type);

#endif /* ! PARSER_INTERNAL_H */
