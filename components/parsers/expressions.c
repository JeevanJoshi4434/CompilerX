#include "../memory.h"
#include "header/parser.h"
#include "header/expressions.h"
#include "../tokens.h"  

extern Token* current;
extern void nextToken();

ASTNode* factor();
ASTNode* primary();
ASTNode* multiplicative();
ASTNode* additive();
ASTNode* comparison();
ASTNode* logical();
ASTNode* parseExpression(int minPrecedence);
ASTNode* functionCall(const char* name);

ASTNode* primary() {
    if (current && current->type == LPAREN) {
        nextToken();
        ASTNode* node = parseExpression(0);
        
        if (current && current->type == RPAREN) {
            nextToken();
        } else {
            printf("Error: Expected closing parenthesis\n");
            exit(1);
        }
        
        return node;
    }
    
    return factor();
}

ASTNode* factor() {
    if (!current) {
        printf("Error: Unexpected end of input\n");
        exit(1);
    }
    if (current->type == NUMBER) {
        ASTNode* node = allocateNode(NODE_NUMBER);
        node->number = atoi(current->value);
        nextToken();
        return node;
    } else if (current->type == STRING_LITERAL) {
        ASTNode* node = allocateNode(NODE_STRING_LITERAL);
        strcpy(node->stringLiteral.value, current->value);
        nextToken();
        return node;
    } else if (current->type == BOOLEAN_LITERAL) {
        ASTNode* node = allocateNode(NODE_BOOLEAN_LITERAL);
        strcpy(node->booleanLiteral.value, current->value);
        nextToken();
        return node;
    } else if (current->type == ID) {
        char name[MAX_VAR_NAME_LENGTH];
        strcpy(name, current->value);
        nextToken();
        if (current && current->type == LPAREN) {
            return functionCall(name);
        }
        ASTNode* node = allocateNode(NODE_VAR_REF);
        strcpy(node->varRef.name, name);
        return node;
    } else {
        printf("Error: Unexpected token '%s'\n", current->value);
        exit(1);
    }
}

/**
 * @brief  Parses a term expression.
 * 
 * This function helps with parsing by parsing a term expression. (* and /) 
 *
 * @return The parsed term expression
 */
ASTNode* term() {
    ASTNode *left = factor();
    while (current && current->type == OPERATOR && (current->value[0] == '*' || current->value[0] == '/')) {
        char op = current->value[0];
        nextToken();
        ASTNode *right = factor();
        ASTNode *node = allocateNode(NODE_BINARY_OP);
        node->binaryOp.op = op;
        node->binaryOp.left = left;
        node->binaryOp.right = right;
        left = node;
    }
    return left;
}


/**
 * @deprecated Use parseExpression instead, which is more general to handle cases like 4 + 2 * 3 --> 4 + (2 * 3)
 * @brief  Parses an expression.
 * 
 * This function helps with parsing by parsing an expression. (+ and -)
 * 
 * @return The parsed expression
 */
ASTNode* expression() {
    ASTNode *left = term();
    while (current && current->type == OPERATOR && (current->value[0] == '+' || current->value[0] == '-')) {
        char op = current->value[0];
        nextToken();
        ASTNode *right = term();
        ASTNode *node = allocateNode(NODE_BINARY_OP);
        node->binaryOp.op = op;
        node->binaryOp.left = left;
        node->binaryOp.right = right;
        left = node;
    }
    return left;
}

/**
 * @brief  Parses an expression.
 * 
 * This function helps with parsing by parsing an expression. (+, -, *, /, ^)
 * The precedence of each operator is determined by the getPrecedence function which helps with parsing to determine the order of operations.
 * 
 * @return The parsed expression ASTNode pointer*
 */
ASTNode* parseExpression(int minPrecedence) {
    ASTNode *left = factor();

    while (current && current->type == OPERATOR && getPrecedence(current->value[0]) >= minPrecedence) {
        char op = current->value[0];
        int precedence = getPrecedence(op);
        nextToken();

        if (isRightAssociative(op)) {
            precedence++;
        }

        ASTNode *right = parseExpression(precedence);

        ASTNode *node = allocateNode(NODE_BINARY_OP);
        node->binaryOp.op = op;
        node->binaryOp.left = left;
        node->binaryOp.right = right;
        left = node; 
    }

    return left;
}

ASTNode* parseCondition(int minPrecedence) {
    printf("Parsing condition, current token: %s, type: %s\n", 
           current ? current->value : "NULL", 
           current ? tokenTypeToString(current->type) : "NULL");
    
    ASTNode *left = parseExpression(0); 
    
    printf("After parsing left expression, current token: %s, type: %s\n", 
           current ? current->value : "NULL", 
           current ? tokenTypeToString(current->type) : "NULL");
    if (current && current->type == RELOP) {
        printf("Found relational operator: %s\n", current->value);
        
        char op[3];
        strcpy(op, current->value);
        nextToken();
        
        printf("After consuming relational operator, current token: %s, type: %s\n", 
               current ? current->value : "NULL", 
               current ? tokenTypeToString(current->type) : "NULL");
        
        ASTNode *right = parseExpression(0);
        
        ASTNode *node = allocateNode(NODE_RELATIONAL_OP);
        strcpy(node->relOp.op, op);
        node->relOp.left = left;
        node->relOp.right = right;
        
        left = node;  
    }
    while (current && (current->type == AND || current->type == OR)) {
        printf("Found logical operator: %s\n", current->value);
        
        TokenType opType = current->type;
        nextToken();
        ASTNode *right = parseCondition(0);
        
        ASTNode *node = allocateNode(NODE_LOGICAL_OP);
        node->logicalOp.op[0] = (opType == AND) ? '&' : '|';
        node->logicalOp.op[1] = (opType == AND) ? '&' : '|';
        node->logicalOp.op[2] = '\0';
        node->logicalOp.left = left;
        node->logicalOp.right = right;
        
        left = node;  
    }
    
    return left;
}



int getPrecedence(char op) {
    if (op == '+' || op == '-') return 5;
    if (op == '*' || op == '/') return 6;
    if (op == '^') return 7;
    if (op == '<' || op == '>' || op == '=') return 4;
    return 0;
}


int isRightAssociative(char op) {
    return (op == '^');
}


ASTNode* logical() {
    ASTNode* left = comparison();
    
    while (current && current->type == LOGICAL_OP) {
        char op[3];
        strcpy(op, current->value);
        nextToken();
        
        ASTNode* node = allocateNode(NODE_LOGICAL_OP);
        strcpy(node->logicalOp.op, op);
        node->logicalOp.left = left;
        node->logicalOp.right = comparison();
        
        left = node;
    }
    
    return left;
}

ASTNode* comparison() {
    ASTNode* left = additive();
    
    if (current && current->type == RELATIONAL_OP) {
        char op[3];
        strcpy(op, current->value);
        nextToken();
        
        ASTNode* node = allocateNode(NODE_RELATIONAL_OP);
        strcpy(node->relOp.op, op);
        node->relOp.left = left;
        node->relOp.right = additive();
        
        return node;
    }
    
    return left;
}


int isLogicalOp(const char *op) {
    return strcmp(op, "&&") == 0 || strcmp(op, "||") == 0;
}

int isRelationalOp(const char *op) {
    return strcmp(op, "<") == 0 || strcmp(op, ">") == 0 ||
           strcmp(op, "<=") == 0 || strcmp(op, ">=") == 0 ||
           strcmp(op, "==") == 0 || strcmp(op, "!=") == 0;
}

int isComparisonOperator(const char *value) {
    return strcmp(value, "==") == 0 || 
           strcmp(value, "!=") == 0 || 
           strcmp(value, "<") == 0 || 
           strcmp(value, ">") == 0 || 
           strcmp(value, "<=") == 0 || 
           strcmp(value, ">=") == 0;
}

ASTNode* additive() {
    ASTNode* left = multiplicative();
    
    while (current && current->type == OPERATOR && 
           (current->value[0] == '+' || current->value[0] == '-')) {
        char op = current->value[0];
        nextToken();
        
        ASTNode* node = allocateNode(NODE_BINARY_OP);
        node->binaryOp.op = op;
        node->binaryOp.left = left;
        node->binaryOp.right = multiplicative();
        
        left = node;
    }
    
    return left;
}

ASTNode* multiplicative() {
    ASTNode* left = primary();
    
    while (current && current->type == OPERATOR && 
           (current->value[0] == '*' || current->value[0] == '/')) {
        char op = current->value[0];
        nextToken();
        
        ASTNode* node = allocateNode(NODE_BINARY_OP);
        node->binaryOp.op = op;
        node->binaryOp.left = left;
        node->binaryOp.right = primary();
        
        left = node;
    }
    
    return left;
}
