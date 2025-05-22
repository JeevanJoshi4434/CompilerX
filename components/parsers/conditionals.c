#include "../memory.h"
#include "header/parser.h"
#include "header/parser_functions.h"

/**
 * @brief  Parses a conditional statement with support for else and else-if.
 * 
 * This function helps with parsing a conditional statement. It can handle:
 * - if statement
 * - else statement
 * - nested else-if conditions
 * 
 * @return The parsed conditional statement
 */
ASTNode* conditional() {
    nextToken();
    
    if (current->type != LPAREN) {
        printf("Error: Missing '(' after IF\n");
        exit(1);
    }
    nextToken();
    
    ASTNode *cond = parseCondition(1);
    if (current->type != RPAREN) {
        printf("Error: Missing ')' after condition\n");
        exit(1);
    }
    nextToken();

    if (current->type != LBRACE) {
        printf("Error: Missing '{' after condition\n");
        exit(1);
    }
    nextToken();
    
    ASTNode *thenStmt = statement();
    if (current->type != RBRACE) {
        printf("Error: Missing '}' after IF body\n");
        exit(1);
    }
    nextToken();

    if (current->type == ELSE) {
        nextToken();
        if (current->type == IF) {
            ASTNode *elseIfStmt = conditional();  
            ASTNode *node = allocateNode(NODE_IF);
            if (!node) {
                printf("Memory Error: Failed to allocate memory for else-if node\n");
                exit(1);
            }
            node->ifNode.condition = cond;
            node->ifNode.thenStmt = thenStmt;
            node->ifNode.elseStmt = elseIfStmt;
            return node;
        }
        if (current->type == LBRACE) {
            ASTNode *elseStmt = statement();
            if (current->type != RBRACE) {
                printf("Error: Missing '}' after ELSE body\n");
                exit(1);
            }
            nextToken();
            
            ASTNode *node = allocateNode(NODE_IF);
            if (!node) {
                printf("Memory Error: Failed to allocate memory for else node\n");
                exit(1);
            }
            node->ifNode.condition = cond;
            node->ifNode.thenStmt = thenStmt;
            node->ifNode.elseStmt = elseStmt;
            return node;
        }
    }

    ASTNode *node = allocateNode(NODE_IF);
    if (!node) {
        printf("Memory Error: Failed to allocate memory for if node\n");
        exit(1);
    }
    node->ifNode.condition = cond;
    node->ifNode.thenStmt = thenStmt;
    node->ifNode.elseStmt = NULL;  
    return node;
}
