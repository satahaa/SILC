//
// Created by User on 09-Jul-25.
//

#ifndef SEMANTIC_H
#define SEMANTIC_H
#include "parser.h"

typedef enum {
    SEMANTIC_OK,
    SEMANTIC_ERROR_UNDECLARED_VAR,
    SEMANTIC_ERROR_REDECLARED_VAR,
    SEMANTIC_ERROR_TYPE_MISMATCH,
    SEMANTIC_ERROR_BREAK_OUTSIDE_LOOP,
    SEMANTIC_ERROR_CONTINUE_OUTSIDE_LOOP
} SemanticResult;

typedef struct {
    char name[256];
    VarType type;
    int is_declared;
} SymbolEntry;

typedef struct {
    SymbolEntry* entries;
    int count;
    int capacity;
} Scope;

typedef struct {
    Scope* scopes;
    int scope_count;
    int scope_capacity;
} ScopeStack;

// Initialize semantic analyzer
void semantic_init();

// Analyze the program
SemanticResult semantic_analyze(Program* program);

// Cleanup semantic analyzer
void semantic_cleanup();

#endif //SEMANTIC_H