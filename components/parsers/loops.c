#include "../memory.h"
#include "header/parser.h"
#include "header/parser_functions.h"

/**
 * @brief  Parses a while loop.
 * 
 * This function helps with parsing by parsing a while loop. It can be a while loop.
 * 
 * @return The parsed while loop
 */
ASTNode* loop() {
    nextToken();
    if (current->type != LPAREN) {
        printf("Error: Expected '(' after 'while'\n");
        exit(1);
    }
    nextToken(); 
    ASTNode* condition = parseCondition(1);  
       
    if (current->type != RPAREN) {
        printf("Error: Expected ')' after condition in while loop\n");
        exit(1);
    }
    nextToken(); 
    if (current->type != LBRACE) {
        printf("Error: Expected '{' after while condition\n");
        exit(1);
    }
    nextToken();
     
    ASTNode* body = NULL;
    ASTNode* current_stmt = NULL;
    
    while (current && current->type != RBRACE) {
        ASTNode* stmt = statement();
        
        if (!body) {
            body = stmt;
            current_stmt = stmt;
        } else {
            current_stmt->next = stmt;
            current_stmt = stmt;
        }
    } 
    if (current->type != RBRACE) {
        printf("Error: Expected '}' after while body\n");
        exit(1);
    }
    nextToken();
     
    ASTNode* node = allocateNode(NODE_WHILE);
    if (!node) {
        printf("Memory Error: Failed to allocate memory for while node\n");
        exit(1);
    }
    
    node->whileNode.condition = condition;
    node->whileNode.body = body;
    
    return node;
}

/**
 * @brief  Parses a for loop.
 * 
 * This function helps with parsing by parsing a for loop.
 * 
 * @return The parsed for loop
 */
ASTNode* forLoop() {
    match(FOR);

    if (!current || current->type != LPAREN) {
        printf("Syntax Error: Expected '(' after 'for'\n");
        exit(1);
    }
    match(LPAREN);

    ASTNode *initialization = statement();
    if (!initialization) {
        printf("Syntax Error: Invalid initialization in for loop\n");
        exit(1);
    }

    if (!current || current->type != SEMICOLON) {
        printf("Syntax Error: Expected ';' after initialization in for loop\n");
        exit(1);
    }
    match(SEMICOLON);

    ASTNode *condition = parseCondition(1); 
    if (!condition) {
        printf("Syntax Error: Invalid condition in for loop\n");
        exit(1);
    }

    if (!current || current->type != SEMICOLON) {
        printf("Syntax Error: Expected ';' after condition in for loop\n");
        exit(1);
    }
    match(SEMICOLON);
    ASTNode *increment = parseExpression(1);  
    if (!increment) {
        printf("Syntax Error: Invalid increment in for loop\n");
        exit(1);
    }
    if (!current || current->type != RPAREN) {
        printf("Syntax Error: Expected ')' after loop components in for loop\n");
        exit(1);
    }
    match(RPAREN);
    if (!current || current->type != LBRACE) {
        printf("Syntax Error: Expected '{' before loop body\n");
    }
    match(LBRACE);
    ASTNode *body = statement(); 
    if (!body) {
        printf("Syntax Error: Invalid statement in for loop body\n");
        exit(1);
    }
    if (!current || current->type != RBRACE) {
        printf("Syntax Error: Expected '}' after loop body\n");
        exit(1);
    }
    match(RBRACE);
    ASTNode *node = allocateNode(NODE_FOR);
    if (!node) {
        printf("Memory Error: Failed to allocate memory for for loop node\n");
        exit(1);
    }

    node->forNode.initialization = initialization;
    node->forNode.condition = condition;
    node->forNode.increment = increment;
    node->forNode.body = body;

    return node;
}

/**
 * @brief  Parses a do-while loop.
 * 
 * This function helps with parsing by parsing a do-while loop.
 * 
 * @return The parsed do-while loop
 */
ASTNode* doWhileLoop() {
    
    match(DO);
    if (!current || current->type != LBRACE) {
        printf("Syntax Error: Expected '{' before loop body\n");
        exit(1);
    }
    match(LBRACE);
    ASTNode *body = statement();
    if (!body) {
        printf("Syntax Error: Invalid statement in do-while loop body\n");
        exit(1);
    }

    if (!current || current->type != RBRACE) {
        printf("Syntax Error: Expected '}' after loop body\n");
        exit(1);
    }
    match(RBRACE);

    if (!current || current->type != WHILE) {
        printf("Syntax Error: Expected 'while'\n");
        exit(1);
    }
    match(WHILE);
    if (!current || current->type != LPAREN) {
        printf("Syntax Error: Expected '(' after 'while'\n");
        exit(1);
    }
    match(LPAREN);
    ASTNode *condition = parseCondition(1);
    if (!condition) {
        printf("Syntax Error: Invalid condition in do-while loop\n");
        exit(1);
    }

    if (!current || current->type != RPAREN) {
        printf("Syntax Error: Expected ')' after condition in do-while loop\n");
        exit(1);
    }
    match(RPAREN);
    ASTNode *node = allocateNode(NODE_DO_WHILE);
    if (!node) {
        printf("Memory Error: Failed to allocate memory for do-while loop node\n");
        exit(1);
    }
    node->doWhileNode.condition = condition;
    node->doWhileNode.body = body;

    return node;
}
