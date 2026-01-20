#ifndef PRETTY_PRINTER_H
#define PRETTY_PRINTER_H

#include <stdio.h>

#include "../../ast/ast.h"

void ast_pretty_print(const struct ast *ast, FILE *out);

#endif /* ! PRETTY_PRINTER_H */