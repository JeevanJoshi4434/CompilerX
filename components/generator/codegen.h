#ifndef CODEGEN_H
#define CODEGEN_H

#include "../ast.h"

void generateAssembly(const char *filename);

void generateCode(ASTNode *node, FILE *asmFile);

void codegenResetVisited();

#endif // CODEGEN_H