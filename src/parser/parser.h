#ifndef PARSER_H
#define PARSER_H

#include "../ast/ast.h"

int parse_error_occurred(void);
void parse_set_error(void);

struct ast *parse_input(FILE *f);

#endif /* ! PARSER_H */
