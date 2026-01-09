#ifndef PARSER_INTERNAL_H
#define PARSER_INTERNAL_H

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../lexer/token/token.h"

typedef struct parser
{
    lexer_t lexer;
    token_t *current_token;
} parser_t;

token_type_t peek(parser_t *parser);
void pop(parser_t *parser);

int is_semicolon_newline(token_type_t t);
void skip_semicolon_newline(parser_t *parser);

ast_t *parse_simple_command(parser_t *parser);
ast_t *parse_list(parser_t *parser);
ast_t *parse_command(parser_t *parser);
ast_t *parse_if(parser_t *parser);
ast_t *parse_elif(parser_t *parser);

ast_t *parse_condition(parser_t *parser);
ast_t *parse_then(parser_t *parser);
ast_t *parse_else(parser_t *parser);

#endif /* ! PARSER_INTERNAL_H */
