#include <stdlib.h>
#include <string.h>
#include "parser.h"

static Token current_token;

void parser_init() {
    const Token null_token = {TOKEN_UNKNOWN, nullptr, 0, 0};
    current_token = lexer_next_token(null_token);
}

static void eat(const Ttype type) {
    if (current_token.type == type) {
        token_free(&current_token);
        current_token = lexer_next_token(current_token);
    } else {
        fprintf(stderr, "Syntax error: Expected %s but got %s at line %d, column %d\n",
                token_type_to_string(type),
                token_type_to_string(current_token.type),
                current_token.line,
                current_token.column);
        exit(EXIT_FAILURE);
    }
}
static Expression* parse_expression() {
    Expression* expr = malloc(sizeof(Expression));
    expr->token_types = malloc(32 * sizeof(Ttype));
    expr->token_values = malloc(32 * sizeof(char*));
    expr->len = 0;
    int paren_count = 0;

    // Handle empty expressions
    if (current_token.type == TOKEN_SEMICOLON) {
        return expr;
    }

    // Parse tokens until semicolon or unexpected token
    while (current_token.type != TOKEN_SEMICOLON) {
        // Check for expression elements
        if (current_token.type == TOKEN_NUMBER ||
            current_token.type == TOKEN_IDENT ||
            current_token.type == TOKEN_PLUS ||
            current_token.type == TOKEN_MINUS ||
            current_token.type == TOKEN_MUL ||
            current_token.type == TOKEN_DIV ||
            current_token.type == TOKEN_LPAREN ||
            current_token.type == TOKEN_RPAREN) {

            // Track parentheses balance
            if (current_token.type == TOKEN_LPAREN) {
                paren_count++;
            } else if (current_token.type == TOKEN_RPAREN) {
                paren_count--;
                if (paren_count < 0) {
                    fprintf(stderr, "Syntax error: Unbalanced parentheses at line %d, column %d\n",
                           current_token.line, current_token.column);
                    exit(EXIT_FAILURE);
                }
            }

            // Store token information
            expr->token_types[expr->len] = current_token.type;
            expr->token_values[expr->len] = strdup(current_token.value);
            expr->len++;

            // Get next token
            eat(current_token.type);
            } else break;
    }

    // Check for balanced parentheses
    if (paren_count != 0) {
        fprintf(stderr, "Syntax error: Unbalanced parentheses\n");
        exit(EXIT_FAILURE);
    }

    return expr;
}

static Statement parse_return_statement() {
    Statement stmt;
    stmt.type = STMT_RETURN;

    // Initialize fields
    stmt.ret_stmt.ident = nullptr;
    stmt.ret_stmt.value = 0;
    stmt.ret_stmt.expr = nullptr;

    // Consume 'ret' token
    eat(TOKEN_RETURN);

    // Check if there's an expression before semicolon
    if (current_token.type != TOKEN_SEMICOLON) {
        // Parse the return value
        if (current_token.type == TOKEN_IDENT) {

            stmt.ret_stmt.ident = strdup(current_token.value);
            eat(TOKEN_IDENT);

        }
        else if (current_token.type == TOKEN_NUMBER) {

            errno = 0;
            char *end_ptr;
            stmt.ret_stmt.value = strtol(current_token.value, &end_ptr, 10);
            if (errno != 0 || *end_ptr != '\0') {
                fprintf(stderr, "Error: Invalid number at line %d, column %d\n",
                        current_token.line, current_token.column);
                exit(EXIT_FAILURE);
            }

        }
        else {
            // Parse complex expression
            stmt.ret_stmt.expr = parse_expression();
        }
    }

    // Expect semicolon
    eat(TOKEN_SEMICOLON);
    return stmt;
}
static Statement parse_let_statement() {
    Statement stmt;
    stmt.type = STMT_LET;

    eat(TOKEN_LET);
    stmt.let_stmt.ident = strdup(current_token.value);
    eat(TOKEN_IDENT);
    eat(TOKEN_EQ);

    // Parse expression
    stmt.let_stmt.expr = parse_expression();
    stmt.let_stmt.ident_value = nullptr;
    stmt.let_stmt.value = 0;

    // For backward compatibility, if expression is just a number or identifier
    if (stmt.let_stmt.expr->len == 1) {

        if (stmt.let_stmt.expr->token_types[0] == TOKEN_NUMBER) {

            errno = 0;
            char *end_ptr;
            stmt.let_stmt.value = strtol(stmt.let_stmt.expr->token_values[0], &end_ptr, 10);
            if (errno != 0 || *end_ptr != '\0') {
                fprintf(stderr, "Error: Invalid number in expression\n");
                exit(EXIT_FAILURE);
            }
            expression_free(stmt.let_stmt.expr);
            stmt.let_stmt.expr = nullptr;

        } else if (stmt.let_stmt.expr->token_types[0] == TOKEN_IDENT) {

            stmt.let_stmt.ident_value = strdup(stmt.let_stmt.expr->token_values[0]);
            expression_free(stmt.let_stmt.expr);
            stmt.let_stmt.expr = nullptr;

        }

    }
    // Don't error on complex expressions - keep them in expr

    eat(TOKEN_SEMICOLON);
    return stmt;
}

Program parser_parse() {
    Program program;
    program.count = 0;
    program.capacity = 10;
    program.statements = malloc(program.capacity * sizeof(Statement));

    while (current_token.type != TOKEN_EOF) {
        Statement stmt;

        switch (current_token.type) {
            case TOKEN_RETURN:
                stmt = parse_return_statement();
                break;
            case TOKEN_LET:
                stmt = parse_let_statement();
                break;
            default:
                fprintf(stderr, "Syntax error: Unexpected token %s at line %d, column %d\n",
                        token_type_to_string(current_token.type),
                        current_token.line,
                        current_token.column);
                exit(1);
        }

        if (program.count >= program.capacity) {
            program.capacity *= 2;
            Statement* tmp = realloc(program.statements, program.capacity * sizeof(Statement));
            if (tmp == NULL) {
                free(program.statements);
                fprintf(stderr, "Memory allocation error\n");
                exit(1);
            }
            program.statements = tmp;
        }

        program.statements[program.count++] = stmt;
    }

    return program;
}

void parser_cleanup() {
    token_free(&current_token);
}

void program_free(Program* program) {
    free(program->statements);
    program->statements = nullptr;
    program->count = 0;
    program->capacity = 0;
}

void expression_free(Expression* expr) {
    if (!expr) return;

    if (expr->token_values) {
        for (int i = 0; i < expr->len; i++) {
            if (expr->token_values[i] != NULL) {
                free(expr->token_values[i]);
            }
        }
        free(expr->token_values);
    }

    if (expr->token_types) {
        free(expr->token_types);
    }

    free(expr);
}