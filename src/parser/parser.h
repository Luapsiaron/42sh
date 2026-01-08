#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"

// 1st parser to call if no lexer already exist, it create the lexer
// and call the 2nd parser
ast_t parser_init(FILE *f, char *string);
ast_t parser(lexer_t lx);

#endif /* ! PARSER_H */