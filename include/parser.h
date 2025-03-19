#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

typedef enum {
    STMT_RETURN, STMT_LET,
} StatementType;

typedef struct {
    Ttype* token_types;
    char** token_values;
    int len;
} Expression;

typedef struct {
    int value;
    char* ident;
    Expression* expr;
} ReturnStatement;

typedef struct {
    char* ident;
    int value;
    char* ident_value;
    Expression* expr;
} LetStatement;

typedef struct {
    StatementType type;
    union {
        ReturnStatement ret_stmt;
        LetStatement let_stmt;
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

// Parse an Expression
static Expression* parse_expression();

//Free the resources used by the expression
void expression_free(Expression* expr);


#endif // PARSER_H