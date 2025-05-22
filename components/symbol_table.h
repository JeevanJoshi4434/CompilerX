#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#define MAX_SYMBOLS 100
#define MAX_VAR_NAME_LENGTH 50

typedef enum {
    TYPE_UNKNOWN = -1,
    TYPE_NUMBER,
    TYPE_STRING,
    TYPE_BOOLEAN
} VariableType;

typedef struct {
    char name[MAX_VAR_NAME_LENGTH];
    int value;
    VariableType type;
} Symbol;

extern Symbol symTable[MAX_SYMBOLS];
extern int symCount;

void initSymbolTable();
int insertSymbol(const char* name, int value, VariableType type);
int lookupSymbol(const char* name);
int getSymbolValue(const char* name);
VariableType getSymbolType(const char* name); 

#endif // SYMBOL_TABLE_H
