#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

typedef enum {
    STMT_RETURN
} StatementType;

typedef struct {
    int value;
} ReturnStatement;

typedef struct {
    StatementType type;
    union {
        ReturnStatement ret_stmt;
    };
} Statement;

typedef struct {
    Statement* statements;
    int count;
    int capacity;
} Program;

// Initialize the parser
void parser_init();

// Parse the tokens into an AST
Program parser_parse();

// Free the resources used by the parser
void parser_cleanup();

// Free the resources used by the program
void program_free(Program* program);

#endif // PARSER_H