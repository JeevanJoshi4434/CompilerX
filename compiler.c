#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "utils/help.c"
#include "components/symbol_table.h"
#include "parser.h"
#include "components/parsers/header/parser.h"
#include "components/ast_visualizer.h"
#include "components/ast_json_exporter.h"
#include "semantic.h"

extern ASTNode *astHead;

struct Stack;

extern Token* current;
extern Token* head;
extern void tokenize(FILE *file);
extern ASTNode* statement();
extern void freeArena();
extern struct Stack bracesStack;
extern struct Stack parenStack;
extern int isEmpty(struct Stack *stack);

int statementCount = 0;
int verboseOutput = 0;

/**
 * @brief Compiles the given file.
 * 
 * @param filename The name of the file to compile.
 * @return void
 */
void compileFile(const char* filename) {
    initSymbolTable();
    
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file '%s'\n", filename);
        exit(1);
    }

    char outputFile[256];
    strncpy(outputFile, filename, sizeof(outputFile) - 5);
    outputFile[sizeof(outputFile) - 5] = '\0';
    
    char *dotPos = strrchr(outputFile, '.');
    if (dotPos) {
        *dotPos = '\0';
    }
    strcat(outputFile, ".asm");
    
    if (verboseOutput) {
        printf("Output file will be: %s\n", outputFile);
    } else {
        printf("Compiling %s to %s\n", filename, outputFile);
    }

    char source[10000] = {0}; 
    char line[1000];
    while (fgets(line, sizeof(line), file)) {
        if (strlen(source) + strlen(line) >= sizeof(source) - 1) {
            printf("Error: Source file too large for buffer\n");
            fclose(file);
            exit(1);
        }
        strcat(source, line);
    }
    tokenize(file);
    fclose(file);
    
    current = head;
    if (!current) {
        printf("Error: No tokens to parse\n");
        exit(1);
    }
    
    #define MAX_STATEMENTS 100
    ASTNode* statements[MAX_STATEMENTS];
    ASTNode* prevNode = NULL;
    astHead = NULL;
    
    int statementCount = 0;
    
    while (current && current->type != END_OF_FILE && statementCount < MAX_STATEMENTS) {
        ASTNode* node = statement();
        if (!node) {
            printf("Warning: statement() returned NULL\n");
            continue;
        }
        statements[statementCount++] = node;
        if (statementCount == 1) {
            astHead = node;
            printf("AST head set to node type: %d\n", node->type);
        } else if (prevNode) {
            prevNode->next = node;
            printf("Linked node %d to node %d\n", statementCount-2, statementCount-1);
        }
        prevNode = node;
    }

    #include "components/ast_json_exporter.h"

    #ifdef _WIN32
    system("mkdir ast_json 2>nul");
    #else
    system("mkdir -p ast_json");
    #endif

    exportASTsToSingleJSON(statements, statementCount, "ast_json/ast_all_statements.json");

    if (!isEmpty(&bracesStack)) {
        printf("Syntax Error: Unclosed braces '{' detected\n");
        exit(1);
    }
    if (!isEmpty(&parenStack)) {
        printf("Syntax Error: Unclosed parentheses '(' detected\n");
        exit(1);
    }
    
    printf("\nParsing completed. Starting semantic analysis...\n");
    
    if (!astHead) {
        printf("Error: AST is empty. No code to analyze.\n");
        exit(1);
    } else {
        printf("AST head is not NULL, type: %d\n", astHead->type);
        
        ASTNode* temp = astHead;
        int count = 0;
        while (temp && count < 5) {
            printf("Node %d: type %d\n", count, temp->type);
            temp = temp->next;
            count++;
        }
    }
    analyzeAndGenerateCode(astHead, outputFile);
    
    printf("\nCompilation completed successfully.\n");
    freeArena(); 
}


int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("Usage: %s <filename.cx>\n", argv[0]);
        return 1;
    }
    if (strcmp(argv[1], "-help") == 0 || strcmp(argv[1], "--help") == 0 || 
        strcmp(argv[1], "-h") == 0 || strcmp(argv[1], "--h") == 0 || 
        strcmp(argv[1], "-H") == 0 || strcmp(argv[1], "--H") == 0) {
        help();
        exit(0);
    }
    
    char *filename = argv[1];
    char *dot = strrchr(filename, '.'); 

    if (dot == NULL || strcmp(dot, ".cx") != 0) {
        printf("Error: Invalid file extension\n");
        exit(1);
    }

    compileFile(argv[1]);

    return 0;
}
