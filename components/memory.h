#ifndef MEMORY_H
#define MEMORY_H

#include "ast.h"

void* arenaAlloc(size_t size);
ASTNode* allocateNode(NodeType type);
void freeArena();

extern ASTNode* astHead;
extern ASTNode* astTail;
extern Arena* arenaHead;

#endif // MEMORY_H
