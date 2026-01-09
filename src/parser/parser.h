#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"

// 1st parser to call if no lexer already exist, it create the lexer
// and call the 2nd parser
ast_t parser_init(FILE *f, char *string);
ast_t *parse_input(FILE *f);

// call in the same file but need to be defined for upper fonction
ast_t *parse_elif_command(void);
ast_t *parse_command(void);

#endif /* ! PARSER_H */