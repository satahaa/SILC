#ifndef PARSER_H
#define PARSER_H
#include "lexer.h"

// Forward declaration first
typedef struct Statement Statement;

typedef enum {
    STMT_RETURN, STMT_LET, STMT_IF, STMT_OUT, STMT_EXPR, STMT_FOR,
} StatementType;

typedef enum { TYPE_DOUBLE, TYPE_STRING } VarType;
typedef struct {
    char name[256];
    VarType type;
} Symbol;


typedef struct {
    Ttype* token_types;
    char** token_values;
    int len;
} Expression;

typedef struct {
    Expression* expr;
} ExpressionStatement;

typedef struct {
    Expression* expr;
} ReturnStatement;

typedef struct {
    char* ident;
    Expression* expr;
} LetStatement;

typedef struct {
    Expression* condition;
    Statement* if_block;
    int if_count;
    Statement* else_block;
    int else_count;
} IfStatement;

typedef struct {
    Expression* expr;
} OutStatement;

typedef struct {
    Expression* start_expr;
    Expression* end_expr;
    Statement* body;
    int body_count;
} ForStatement;

typedef struct Statement {
    StatementType type;
    union {
        ReturnStatement ret_stmt;
        LetStatement let_stmt;
        IfStatement if_stmt;
        OutStatement out_stmt;
        ExpressionStatement expr_stmt;
        ForStatement for_stmt;
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

static Statement parse_expression_statement();
static Statement parse_if_statement();
static Statement parse_out_statement();
static Program parser_parse_block();
static Program parse_block_statements();
static Statement parse_for_statement();
void if_statement_free(const IfStatement* if_stmt);
void for_statement_free(const ForStatement* for_stmt);
#endif // PARSER_H
