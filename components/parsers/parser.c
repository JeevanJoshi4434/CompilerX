#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "header/parser.h"
#include "../memory.h"
#include "../symbol_table.h"

#define MAX_STACK_SIZE 100

struct Stack bracesStack = { .top = -1 };
struct Stack parenStack = { .top = -1 };

void push(struct Stack *stack, char item) {
    if (stack->top >= MAX_STACK_SIZE - 1) {
        printf("Error: Stack overflow\n");
        exit(1);
    }
    stack->items[++(stack->top)] = item;
}

char pop(struct Stack *stack) {
    if (stack->top < 0) {
        return '\0'; 
    }
    return stack->items[(stack->top)--];
}

int isEmpty(struct Stack *stack) {
    return stack->top < 0;
}

void nextToken() {
    if (current) {
        current = current->next;
        if (current) {
            if (current->type == LBRACE) {
                push(&bracesStack, '{');
            } 
            else if (current->type == RBRACE) {
                char opener = pop(&bracesStack);
                if (opener != '{') {
                    printf("Syntax Error: Unmatched closing brace '}'\n");
                    exit(1);
                }
            }
            else if (current->type == LPAREN) {
                push(&parenStack, '(');
            }
            else if (current->type == RPAREN) {
                char opener = pop(&parenStack);
                if (opener != '(') {
                    printf("Syntax Error: Unmatched closing parenthesis ')'\n");
                    exit(1);
                }
            }
        }
        
        printf("Next token is type: %s , value: %s\n", 
               current ? tokenTypeToString(current->type) : "NULL", 
               current ? current->value : "NULL");
    }
}

void match(TokenType expected) {
    if (current && current->type == expected)
    {
        nextToken();
    }
    else
    {
        printf("Syntax Error: Unexpected token '%s'\n", current ? current->value : "NULL");
        exit(1);
    }
}

void expect(TokenType expected) {
    if (current && current->type != expected)
    {
        printf("Syntax Error: Expected '%s' but found '%s'\n", tokenTypeToString(expected), tokenTypeToString(current->type));
        exit(1);
    }else{
    nextToken();
    }
}
