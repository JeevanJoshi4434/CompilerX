#ifndef SEMANTIC_H
#define SEMANTIC_H

#include "components/ast.h"

void checkSemantic(ASTNode *node);

void analyzeAndGenerateCode(ASTNode *root, const char *outputFile);

int getExprType(ASTNode *expr);

#endif // SEMANTIC_H
