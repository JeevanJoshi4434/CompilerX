#include "semantic.h"
#include "components/generator/codegen.h"
#include "components/ast.h"
#include "components/symbol_table.h"
#include <stdio.h>
#include <stdlib.h>

#define MAX_VISITED_NODES 1000
ASTNode* semanticVisitedNodes[MAX_VISITED_NODES];
int semanticVisitedCount = 0;

int semanticHasVisited(ASTNode* node) {
    printf("Checking if node %p has been visited before\n", (void*)node);
    for (int i = 0; i < semanticVisitedCount; i++) {
        if (semanticVisitedNodes[i] == node) {
            printf("Node %p has been visited before\n", (void*)node);
            return 1;
        }
    }
    printf("Node %p has not been visited before\n", (void*)node);
    return 0;
}

void semanticMarkVisited(ASTNode* node) {
    if (semanticVisitedCount < MAX_VISITED_NODES) {
        semanticVisitedNodes[semanticVisitedCount++] = node;
    }
}

void semanticResetVisited() {
    semanticVisitedCount = 0;
}

const char* typeToString(int type) {
    switch(type) {
        case TYPE_NUMBER: return "number";
        case TYPE_STRING: return "string";
        case TYPE_BOOLEAN: return "boolean";
        default: return "unknown";
    }
}

void dumpSymbolTable() {
    printf("=== SYMBOL TABLE DUMP ===\n");
    for (int i = 0; i < symCount; i++) {
        printf("Symbol[%d]: name='%s', type=%d, value=%d\n", 
               i, symTable[i].name, symTable[i].type, symTable[i].value);
    }
    printf("========================\n");
}

void checkSemantic(ASTNode *node) {
    if (!node) {
        printf("Warning: Null node encountered in semantic analysis\n");
        return;
    }

    dumpSymbolTable();

    if (semanticHasVisited(node)) {
        printf("Warning: Circular reference detected in AST. Skipping node.\n");
        return;
    }
    
    semanticMarkVisited(node);

    printf("Checking node type: %d\n", node->type);

    switch (node->type) {
        case NODE_ASSIGN:
            printf("Checking assignment to: %s\n", node->assign.name);
            if (lookupSymbol(node->assign.name) == -1) { 
                printf("Semantic Error: Variable '%s' not declared before assignment\n", node->assign.name);
                exit(1);
            }
            printf("Variable '%s' found in symbol table\n", node->assign.name);
            
            if (node->assign.expr) {
                int exprType = getExprType(node->assign.expr);
                int varType = getSymbolType(node->assign.name);
                printf("Variable type: %d, Expression type: %d\n", varType, exprType);
                
                if (varType != exprType && exprType != TYPE_UNKNOWN) {
                    printf("Semantic Error: Type mismatch in assignment to '%s'. Cannot assign %s to %s\n", 
                           node->assign.name, 
                           typeToString(exprType), 
                           typeToString(varType));
                    exit(1);
                }
            } else {
                printf("Warning: Null expression in assignment\n");
            }
            
            if (node->assign.expr) {
                checkSemantic(node->assign.expr);
            }
            break;

        case NODE_VAR_DECL:
            printf("Checking variable declaration: %s\n", node->varDecl.name);
            
            if (node->varDecl.value) {
                int declaredType = node->varDecl.type;
                int valueType = getExprType(node->varDecl.value);
                
                printf("Variable type: %d, Value type: %d\n", declaredType, valueType);
                
                if (declaredType != valueType && valueType != TYPE_UNKNOWN) {
                    printf("Semantic Error: Type mismatch in declaration of '%s'. Cannot assign %s to %s\n", 
                           node->varDecl.name, 
                           typeToString(valueType), 
                           typeToString(declaredType));
                    exit(1);
                }
                
                checkSemantic(node->varDecl.value);
            }
            break;

        case NODE_FUNC_CALL:
            printf("Checking function call: %s\n", node->funcCall.name);
            if (lookupSymbol(node->funcCall.name) == -1) { 
                printf("Semantic Error: Function '%s' is not defined\n", node->funcCall.name);
                exit(1);
            }
            
            printf("Function '%s' found in symbol table\n", node->funcCall.name);
            
            if (node->funcCall.args) {
                checkSemantic(node->funcCall.args);
            }
            break;

        case NODE_FUNC_DEF:
            printf("Checking function definition: %s\n", node->funcDef.name);
            if (node->funcDef.body) {
                checkSemantic(node->funcDef.body);
            }
            break;

        case NODE_IF:
            printf("Checking if statement\n");
            if (node->ifNode.condition) {
                checkSemantic(node->ifNode.condition);
            }
            if (node->ifNode.thenStmt) {
                checkSemantic(node->ifNode.thenStmt);
            }
            if (node->ifNode.elseStmt) {
                checkSemantic(node->ifNode.elseStmt);
            }
            break;

        case NODE_WHILE:
            printf("Checking while loop\n");
            if (node->whileNode.condition) {
                checkSemantic(node->whileNode.condition);
            }
            if (node->whileNode.body) {
                checkSemantic(node->whileNode.body);
            }
            break;

        case NODE_FOR:
            printf("Checking for loop\n");
            if (node->forNode.initialization) {
                checkSemantic(node->forNode.initialization);
            }
            if (node->forNode.condition) {
                checkSemantic(node->forNode.condition);
            }
            if (node->forNode.increment) {
                checkSemantic(node->forNode.increment);
            }
            if (node->forNode.body) {
                checkSemantic(node->forNode.body);
            }
            break;

        case NODE_DO_WHILE:
            printf("Checking do-while loop\n");
            if (node->doWhileNode.body) {
                checkSemantic(node->doWhileNode.body);
            }
            if (node->doWhileNode.condition) {
                checkSemantic(node->doWhileNode.condition);
            }
            break;

        case NODE_PRINT:
            printf("Checking print statement\n");
            if (node->print.expr) {
                checkSemantic(node->print.expr);
            }
            break;

        case NODE_BINARY_OP:
            printf("Checking binary operation: %c\n", node->binaryOp.op);
            if (node->binaryOp.left) {
                checkSemantic(node->binaryOp.left);
            }
            if (node->binaryOp.right) {
                checkSemantic(node->binaryOp.right);
            }
            break;

        case NODE_NUMBER:
            printf("Checking number: %d\n", node->number);
            break;

        case NODE_STRING_LITERAL:
            printf("Checking string literal: %s\n", node->stringLiteral.value);
            break;

        default:
            printf("Warning: Unhandled node type %d in semantic analysis\n", node->type);
            break;
    }

    // Check the next node in the list
    printf("Checking next node\n");
    if (node->next) {
        checkSemantic(node->next);
    } else {
        printf("No more nodes to check\n");
    }
}

/**
 * @brief Analyzes the AST and generates assembly code.
 * 
 * @param root The root node of the AST.
 * @param outputFile The output file name for the assembly code.
 * @return void
 */
void analyzeAndGenerateCode(ASTNode *root, const char *outputFile) {
    printf("Starting semantic analysis...\n");
    
    if (!root) {
        printf("Error: AST is empty. No code to analyze.\n");
        exit(1);
    }
    
    semanticResetVisited();
    
    printf("Checking AST...\n");
    checkSemantic(root);
    
    printf("Semantic analysis completed successfully.\n");
    
    printf("Generating assembly code to: %s\n", outputFile);
    
    generateAssembly(outputFile);
    
    printf("Code generation completed successfully.\n");
}

int getExprType(ASTNode *expr) {
    if (!expr) return TYPE_UNKNOWN;
    
    switch (expr->type) {
        case NODE_NUMBER:
            return TYPE_NUMBER;
        case NODE_STRING_LITERAL:
            return TYPE_STRING;
        case NODE_BOOLEAN_LITERAL:
            return TYPE_BOOLEAN;
        case NODE_VAR_REF:
            return getSymbolType(expr->varRef.name);
        case NODE_BINARY_OP:
            // Binary operations typically result in numbers
            return TYPE_NUMBER;
        default:
            return TYPE_UNKNOWN;
    }
}
