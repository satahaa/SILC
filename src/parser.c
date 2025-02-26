#include "lexer.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
/*
 * This parser handles a simple language with the following grammar:
 *
 * program        → statement* EOF
 * statement      → variableDecl | returnStmt
 * variableDecl   → "let" IDENTIFIER "=" expression ";"
 * returnStmt     → "ret" expression ";"
 * expression     → IDENTIFIER | NUMBER
 */

typedef enum {
    NODE_VARIABLE_DECL,
    NODE_RETURN_STMT,
    NODE_IDENTIFIER,
    NODE_NUMBER
} NodeType;

typedef struct AstNode {
    NodeType type;
    union {
        struct {
            char* name;
            struct AstNode* initializer;
        } varDecl;

        struct {
            struct AstNode* value;
        } returnStmt;

        struct {
            char* name;
        } identifier;

        struct {
            int value;
        } number;
    } as;

    size_t line;
    size_t column;
} AstNode;

typedef struct {
    Lexer* lexer;
    Token* current;
    Token* previous;
    bool hadError;
    bool panicMode;
} Parser;

// Forward declarations
AstNode* parse_program(Parser* parser);
AstNode* parse_statement(Parser* parser);
AstNode* parse_variable_declaration(Parser* parser);
AstNode* parse_return_statement(Parser* parser);
AstNode* parse_expression(Parser* parser);
void free_ast(AstNode* node);

// Initialize parser
Parser* parser_init(Lexer* lexer) {
    Parser* parser = malloc(sizeof(Parser));
    if (!parser) {
        fprintf(stderr, "Memory allocation failed for parser\n");
        exit(EXIT_FAILURE);
    }

    parser->lexer = lexer;
    parser->current = &lexer->tokens[0];
    parser->previous = NULL;
    parser->hadError = false;
    parser->panicMode = false;

    return parser;
}

// Free parser resources
void parser_free(Parser* parser) {
    if (parser) {
        free(parser);
    }
}

// Error reporting
void parser_error(Parser* parser, const char* message) {
    if (parser->panicMode) return;
    parser->panicMode = true;
    parser->hadError = true;

    fprintf(stderr, "[line %zu, column %zu] Error: %s\n",
            parser->current->line, parser->current->column, message);
}

// Check if current token is of given type
bool check(const Parser* parser, const TokenType type) {
    return parser->current->type == type;
}

// Advance to next token
void advance(Parser* parser) {
    parser->previous = parser->current;
    parser->current++;
}

// Consume current token if it matches expected type
bool match(Parser* parser, const TokenType type) {
    if (check(parser, type)) {
        advance(parser);
        return true;
    }
    return false;
}

// Consume token with error checking
void consume(Parser* parser, const TokenType type, const char* message) {
    if (check(parser, type)) {
        advance(parser);
    } else {
        parser_error(parser, message);
    }
}

// Create a new AST node
AstNode* create_node(const NodeType type, const size_t line, const size_t column) {
    AstNode* node = malloc(sizeof(AstNode));
    if (!node) {
        fprintf(stderr, "Memory allocation failed for AST node\n");
        exit(EXIT_FAILURE);
    }

    node->type = type;
    node->line = line;
    node->column = column;

    return node;
}

// Parse a complete program
AstNode* parse_program(Parser* parser) {
    // For simplicity, we'll just return the first statement
    // In a real parser, you'd build a list of statements

    AstNode* stmt = parse_statement(parser);

    consume(parser, TOKEN_EOF, "Expected end of file");

    return stmt;
}

// Parse a statement
AstNode* parse_statement(Parser* parser) {
    if (match(parser, TOKEN_LET)) {
        return parse_variable_declaration(parser);
    } else if (match(parser, TOKEN_RET)) {
        return parse_return_statement(parser);
    } else {
        parser_error(parser, "Expected statement");
        return NULL;
    }
}

// Parse a variable declaration
AstNode* parse_variable_declaration(Parser* parser) {
    // Expect an identifier
    if (!match(parser, TOKEN_IDENTIFIER)) {
        parser_error(parser, "Expected variable name");
        return NULL;
    }

    // Save the identifier name
    char* name = strdup(parser->previous->value);

    // Expect an equals sign
    consume(parser, TOKEN_ASSIGN, "Expected '=' after variable name");

    // Parse the initializer expression
    AstNode* initializer = parse_expression(parser);

    // Expect a semicolon
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after variable declaration");

    // Create the variable declaration node
    AstNode* node = create_node(NODE_VARIABLE_DECL, parser->previous->line, parser->previous->column);
    node->as.varDecl.name = name;
    node->as.varDecl.initializer = initializer;

    return node;
}

// Parse a return statement
AstNode* parse_return_statement(Parser* parser) {
    // Save the token position
    const size_t line = parser->previous->line;
    const size_t column = parser->previous->column;

    // Parse the return value expression
    AstNode* value = parse_expression(parser);

    // Expect a semicolon
    consume(parser, TOKEN_SEMICOLON, "Expected ';' after return statement");

    // Create the return statement node
    AstNode* node = create_node(NODE_RETURN_STMT, line, column);
    node->as.returnStmt.value = value;

    return node;
}

// Parse an expression
AstNode* parse_expression(Parser* parser) {
    if (match(parser, TOKEN_IDENTIFIER)) {
        // Create an identifier node
        AstNode* node = create_node(NODE_IDENTIFIER, parser->previous->line, parser->previous->column);
        node->as.identifier.name = strdup(parser->previous->value);
        return node;
    } else if (match(parser, TOKEN_INT)) {
        // Create a number node
        AstNode* node = create_node(NODE_NUMBER, parser->previous->line, parser->previous->column);
        node->as.number.value = atoi(parser->previous->value);
        return node;
    } else {
        parser_error(parser, "Expected expression");
        return NULL;
    }
}

// Free the AST
void free_ast(AstNode* node) {
    if (!node) return;

    switch (node->type) {
        case NODE_VARIABLE_DECL:
            free(node->as.varDecl.name);
            free_ast(node->as.varDecl.initializer);
            break;

        case NODE_RETURN_STMT:
            free_ast(node->as.returnStmt.value);
            break;

        case NODE_IDENTIFIER:
            free(node->as.identifier.name);
            break;

        case NODE_NUMBER:
            // No dynamic memory to free
            break;
    }

    free(node);
}

// Print the AST for debugging
void print_ast(AstNode* node, int indent) {
    if (!node) return;

    char indentation[100] = {0};
    for (int i = 0; i < indent; i++) {
        strcat(indentation, "  ");
    }

    switch (node->type) {
        case NODE_VARIABLE_DECL:
            printf("%sVARIABLE_DECL %s\n", indentation, node->as.varDecl.name);
            printf("%sInitializer:\n", indentation);
            print_ast(node->as.varDecl.initializer, indent + 1);
            break;

        case NODE_RETURN_STMT:
            printf("%sRETURN_STMT\n", indentation);
            printf("%sValue:\n", indentation);
            print_ast(node->as.returnStmt.value, indent + 1);
            break;

        case NODE_IDENTIFIER:
            printf("%sIDENTIFIER %s\n", indentation, node->as.identifier.name);
            break;

        case NODE_NUMBER:
            printf("%sNUMBER %d\n", indentation, node->as.number.value);
            break;
    }
}