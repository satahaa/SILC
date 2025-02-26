#ifndef PARSER_H
#define PARSER_H

#include "lexer.h"
#include <stdbool.h>

/**
 * AST node types
 */
typedef enum {
    NODE_VARIABLE_DECL,
    NODE_RETURN_STMT,
    NODE_IDENTIFIER,
    NODE_NUMBER
} NodeType;

/**
 * Abstract Syntax Tree node structure
 */
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

/**
 * Parser structure to maintain parser state
 */
typedef struct {
    Lexer* lexer;
    Token* current;
    Token* previous;
    bool hadError;
    bool panicMode;
} Parser;

/**
 * Initialize a new parser with the given lexer
 *
 * @param lexer The lexer containing tokens to parse
 * @return A pointer to the initialized parser
 */
Parser* parser_init(Lexer* lexer);

/**
 * Free all resources associated with the parser
 *
 * @param parser The parser to free
 */
void parser_free(Parser* parser);

/**
 * Report an error at the current token
 *
 * @param parser The parser where the error occurred
 * @param message The error message
 */
void parser_error(Parser* parser, const char* message);

/**
 * Check if the current token is of the given type
 *
 * @param parser The parser to check
 * @param type The token type to check for
 * @return true if the current token is of the given type, false otherwise
 */
bool check(Parser* parser, TokenType type);

/**
 * Advance to the next token
 *
 * @param parser The parser to advance
 */
void advance(Parser* parser);

/**
 * Check if the current token is of the given type and advance if it is
 *
 * @param parser The parser to check
 * @param type The token type to check for
 * @return true if the current token was consumed, false otherwise
 */
bool match(Parser* parser, TokenType type);

/**
 * Consume the current token if it is of the given type, otherwise report an error
 *
 * @param parser The parser to consume from
 * @param type The expected token type
 * @param message The error message if the token doesn't match
 */
void consume(Parser* parser, TokenType type, const char* message);

/**
 * Create a new AST node
 *
 * @param type The type of node to create
 * @param line The line number where the node appears
 * @param column The column number where the node appears
 * @return A pointer to the new node
 */
AstNode* create_node(NodeType type, size_t line, size_t column);

/**
 * Parse a complete program
 *
 * @param parser The parser to parse with
 * @return The AST root node
 */
AstNode* parse_program(Parser* parser);

/**
 * Parse a statement
 *
 * @param parser The parser to parse with
 * @return An AST node representing the statement
 */
AstNode* parse_statement(Parser* parser);

/**
 * Parse a variable declaration
 *
 * @param parser The parser to parse with
 * @return An AST node representing the variable declaration
 */
AstNode* parse_variable_declaration(Parser* parser);

/**
 * Parse a return statement
 *
 * @param parser The parser to parse with
 * @return An AST node representing the return statement
 */
AstNode* parse_return_statement(Parser* parser);

/**
 * Parse an expression
 *
 * @param parser The parser to parse with
 * @return An AST node representing the expression
 */
AstNode* parse_expression(Parser* parser);

/**
 * Free all resources associated with an AST
 *
 * @param node The root node of the AST to free
 */
void free_ast(AstNode* node);

/**
 * Print an AST for debugging
 *
 * @param node The root node of the AST to print
 * @param indent The indentation level
 */
void print_ast(AstNode* node, int indent);

#endif /* PARSER_H */