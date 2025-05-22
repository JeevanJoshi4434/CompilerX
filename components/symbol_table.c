#include "symbol_table.h"
#include <string.h>
#include <stdio.h>

Symbol symTable[MAX_SYMBOLS];
int symCount = 0;

void initSymbolTable() {
    printf("DEBUG: Initializing symbol table...\n");
    symCount = 0;
    for (int i = 0; i < MAX_SYMBOLS; i++) {
        symTable[i].name[0] = '\0';
        symTable[i].value = 0;
        symTable[i].type = TYPE_UNKNOWN;
    }
    printf("DEBUG: Symbol table initialized, symCount = %d\n", symCount);
}

int insertSymbol(const char* name, int value, VariableType type) {
    printf("DEBUG: Inserting symbol: name='%s', value=%d, type=%d\n", name, value, type);
    
    if (symCount >= MAX_SYMBOLS) {
        printf("Error: Symbol table full\n");
        return -1;
    }
    
    strcpy(symTable[symCount].name, name);
    symTable[symCount].value = value;
    symTable[symCount].type = type; 
    
    printf("DEBUG: Symbol inserted at index %d\n", symCount);
    
    return symCount++;
}

int lookupSymbol(const char* name) {
    for (int i = 0; i < symCount; i++) {
        if (strcmp(symTable[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

int getSymbolValue(const char* name) {
    int index = lookupSymbol(name);
    if (index == -1) {
        printf("Error: Symbol '%s' not found\n", name);
        return 0;
    }
    return symTable[index].value;
}

VariableType getSymbolType(const char* name) {
    int index = lookupSymbol(name);
    if (index == -1) {
        printf("Error: Symbol '%s' not found\n", name);
        return TYPE_UNKNOWN;
    }
    return symTable[index].type;
}
