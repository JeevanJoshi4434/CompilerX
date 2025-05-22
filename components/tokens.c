#include "tokens.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>  

Token* head = NULL;
Token* current = NULL;

extern FILE *yyin;
extern int tokenizer();

void addToken(TokenType type, const char* value) {
    Token* token = malloc(sizeof(Token));
    token->type = type;
    strcpy(token->value, value);
    token->next = NULL;
    
    if (!head) {
        head = token;
        current = token;
    } else {
        Token* temp = head;
        while (temp->next) {
            temp = temp->next;
        }
        temp->next = token;
    }
}

const char* tokenTypeToString(TokenType type) {
    switch (type) {
        case UNKNOWN: return "UNKNOWN";
        case NUMBER: return "NUMBER";
        case ID: return "ID";
        case OPERATOR: return "OPERATOR";
        case LOGICAL_OP: return "LOGICAL_OP";
        case RELATIONAL_OP: return "RELATIONAL_OP";
        case LPAREN: return "LPAREN";
        case RPAREN: return "RPAREN";
        case LBRACE: return "LBRACE";
        case RBRACE: return "RBRACE";
        case SEMICOLON: return "SEMICOLON";
        case COMMA: return "COMMA";
        case ASSIGN: return "ASSIGN";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case WHILE: return "WHILE";
        case DO: return "DO";
        case FOR: return "FOR";
        case FUNC: return "FUNC";
        case CALL: return "CALL";
        case RETURN: return "RETURN";
        case VAR: return "VAR";
        case PRINT: return "PRINT";
        case STRING_LITERAL: return "STRING_LITERAL";
        case BOOLEAN_LITERAL: return "BOOLEAN_LITERAL";
        case AND: return "AND";
        case OR: return "OR";
        case RELOP: return "RELOP";
        case END: return "END";
        default: return "UNKNOWN";
    }
}

void tokenize(FILE *file) {
    if (!file) {
        perror("Failed to open source file");
        exit(1);
    }
    rewind(file);
    yyin = file;
    tokenizer();
    
    printf("\nTokenizing completed!\n");
}
