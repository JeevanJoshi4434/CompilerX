#include <stdlib.h>
#include <string.h>
#include "memory.h"

typedef struct MemoryBlock {
    void* block;
    struct MemoryBlock* next;
} MemoryBlock;

ASTNode* astHead = NULL;
ASTNode* astTail = NULL;
MemoryBlock* memoryBlockHead = NULL;
Arena* arenaHead = NULL;

void* arenaAlloc(size_t size) {
    if (size == 0) {
        printf("Warning: Attempted to allocate 0 bytes\n");
        size = 1;
    }
    
    void* block = malloc(size);
    if (!block) {
        printf("Fatal error: Memory allocation failed for %zu bytes\n", size);
        exit(1);
    }
    
    memset(block, 0, size);
    
    MemoryBlock* newBlock = malloc(sizeof(MemoryBlock));
    if (!newBlock) {
        printf("Fatal error: Memory allocation failed for memory block tracking\n");
        free(block); 
        exit(1);
    }
    
    newBlock->block = block;
    newBlock->next = memoryBlockHead;
    memoryBlockHead = newBlock;
    
    return block;
}

ASTNode* allocateNode(NodeType type) {
    ASTNode* node = arenaAlloc(sizeof(ASTNode));
    if (!node) {
        printf("Error: Failed to allocate memory for AST node\n");
        exit(1);
    }
    
    memset(node, 0, sizeof(ASTNode)); 
    node->type = type;
    node->next = NULL;
    
    return node;
}

void freeArena() {
    MemoryBlock* current = memoryBlockHead;
    while (current) {
        MemoryBlock* next = current->next;
        free(current->block);
        free(current);
        current = next;
    }
    
    memoryBlockHead = NULL;
    arenaHead = NULL;
    astHead = NULL;
    astTail = NULL;
}

