#include "../memory.h"
#include "header/parser.h"
#include "header/parser_functions.h"

/**
 * @brief  Parses a function definition.
 *
 * This function helps with parsing by parsing a function definition. It can be a function definition.
 *
 * @return The parsed function definition
 */
ASTNode *functionDef()
{
    char funcName[50];
    strcpy(funcName, current->value);
    nextToken();
    if (current->type != LPAREN)
    {
        printf("Error: Expected '(' after function name\n");
        exit(1);
    }
    nextToken();
    if (current->type != RPAREN)
    {
        printf("Error: Expected ')' after parameters\n");
        exit(1);
    }
    nextToken();
    if (current->type != LBRACE)
    {
        printf("Error: Expected '{' after function declaration\n");
        exit(1);
    }
    nextToken();
    ASTNode *body = statement();
    ASTNode *funcNode = allocateNode(NODE_FUNC_DEF);
    strcpy(funcNode->funcDef.name, funcName);
    funcNode->funcDef.params = NULL;
    funcNode->funcDef.body = body;
    return funcNode;
}

/**
 * @brief  Parses a function call.
 *
 * This function helps with parsing by parsing a function call. It can be a function call.
 *
 * @return The parsed function call
 */
ASTNode *functionCall()
{
    nextToken();
    ASTNode *node = allocateNode(NODE_FUNC_CALL);
    strcpy(node->funcCall.name, current->value);
    nextToken();
    if (current->type != LPAREN)
    {
        printf("Error: Missing '(' after CALL name\n");
        exit(1);
    }
    nextToken();
    node->funcCall.args = parseExpression(1);
    if (current->type != RPAREN)
    {
        printf("Error: Missing ')' after CALL arguments\n");
        exit(1);
    }
    nextToken();
    return node;
}
