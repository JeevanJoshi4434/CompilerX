#include "../memory.h"
#include "./header/parser.h"
#include "./header/loops.h"
#include "./header/expressions.h"

#define MAX_VAR_NAME_LENGTH 50

extern int evaluateExpression(ASTNode *expr);

void updateOrInsertSymbol(const char *name, int value, VariableType type) {
    int index = lookupSymbol(name);
    if (index != -1) {
        symTable[index].value = value;
        symTable[index].type = type;
    } else {
        insertSymbol(name, value, type);  
    }
}

/**
 * @brief Parses a statement.
 * 
 * This function parses variable declarations, assignments, loops, function calls, and conditionals.
 * 
 * @return The parsed statement AST node.
 */
ASTNode* statement() {
    printf("Parsing statement, current token: %s\n", current->value);
    
    ASTNode* node = NULL;
    
    switch (current->type) {
        case ID: {
            int index = lookupSymbol(current->value);
            if (index != -1) {
                char varName[MAX_VAR_NAME_LENGTH];
                strcpy(varName, current->value);
                nextToken();
                if (current->type == ASSIGN) {
                    nextToken();
                    ASTNode *expr = parseExpression(0);
                    if (current->type != SEMICOLON) {
                        printf("Error: Missing ';' after assignment\n");
                        exit(1);
                    }
                    nextToken();
                    node = allocateNode(NODE_ASSIGN);
                    strcpy(node->assign.name, varName);
                    node->assign.expr = expr;
                    printf("Assignment statement parsed successfully\n");
                } else {
                    printf("Error: Unexpected token after variable name\n");
                    exit(1);
                }
            } else {
                printf("Error: Variable '%s' not declared type: %s\n", current->value, tokenTypeToString(current->type));
                exit(1);
            }
            break;
        }

        case VAR: {
            VariableType varType = TYPE_NUMBER;
            if (strcmp(current->value, "str") == 0) {
                varType = TYPE_STRING;
            } else if (strcmp(current->value, "log") == 0) {
                varType = TYPE_BOOLEAN;
            }
            
            nextToken();
            if (current->type != ID) {
                printf("Error: Expected variable name after type declaration\n");
                exit(1);
            }

            char varName[MAX_VAR_NAME_LENGTH];
            strcpy(varName, current->value);
            
            if (lookupSymbol(varName) != -1) {
                printf("Error: Variable '%s' already declared\n", varName);
                exit(1);
            }
            
            nextToken();
            if (current->type != ASSIGN) {
                printf("Error: Expected '=' after variable name\n");
                exit(1);
            }
            nextToken();
            ASTNode *expr = parseExpression(0);
            if (current->type != SEMICOLON) {
                printf("Error: Missing ';' after variable declaration\n");
                exit(1);
            }
            nextToken();
            
            node = allocateNode(NODE_VAR_DECL);
            strcpy(node->varDecl.name, varName);
            node->varDecl.type = varType;
            node->varDecl.value = expr;
            
            insertSymbol(varName, 0, varType);
            
            printf("Variable declaration parsed successfully\n");
            break;
        }

        case IF: {
            node = conditional();
            break;
        }

        case WHILE: {
            node = loop();
            break;
        }

        case DO: {
            node = doWhileLoop();
            break;
        }

        case FOR: {
            node = forLoop();
            break;
        }

        case FUNC: {
            nextToken();
            if (current->type != ID) {
                printf("Error: Expected function name after 'func'\n");
                exit(1);
            }
            node = functionDef();
            break;
        }

        case PRINT: {
            nextToken();
            ASTNode *expr = parseExpression(0);
            if (current->type != SEMICOLON) {
                printf("Error: Missing ';' after print statement\n");
                exit(1);
            }
            nextToken();
            node = allocateNode(NODE_PRINT);
            node->print.expr = expr;
            printf("Statement parsed successfully\n");
            break;
        }

        default: {
            printf("Error: Unexpected statement\n type: %s , value %s \n", tokenTypeToString(current->type) , current->value);
            exit(1);
        }
    }
    
    if (node) {
        if (node->next == node) {
            printf("Warning: Circular reference detected in node. Setting next to NULL.\n");
            node->next = NULL;
        }
        switch (node->type) {
            case NODE_ASSIGN:
                if (node->assign.expr == node) {
                    printf("Warning: Circular reference in assignment expression. Setting to NULL.\n");
                    node->assign.expr = NULL;
                }
                break;
            case NODE_VAR_DECL:
                if (node->varDecl.value == node) {
                    printf("Warning: Circular reference in variable declaration. Setting value to NULL.\n");
                    node->varDecl.value = NULL;
                }
                break;
        }
    }
    
    return node;
}
