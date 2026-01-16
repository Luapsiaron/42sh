#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"
#include "../ast/ast_free.h"

ast_t *parse_input(FILE *f);

#endif /* ! PARSER_H */
