#ifndef PRETTY_PRINTER_H
#define PRETTY_PRINTER_H

#include "../../ast/ast.h"
#include <stdio.h>

void ast_pretty_print(const ast_t *ast, FILE *out);

#endif /* ! PRETTY_PRINTER_H */