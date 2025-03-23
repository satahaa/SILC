#include <stdlib.h>
#include <string.h>
#include "parser.h"

static Token current_token;

void parser_init() {
    current_token = lexer_next_token();
}

static void eat(const Ttype type) {
    if (current_token.type == type) {
        token_free(&current_token);
        current_token = lexer_next_token();
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
    while (current_token.type != TOKEN_SEMICOLON &&
           current_token.type != TOKEN_EOF       &&
           current_token.type != TOKEN_RBRACE) {

        if (current_token.type == TOKEN_NUMBER ||
            current_token.type == TOKEN_IDENT  ||
            current_token.type == TOKEN_PLUS   ||
            current_token.type == TOKEN_MINUS  ||
            current_token.type == TOKEN_MUL    ||
            current_token.type == TOKEN_DIV    ||
            current_token.type == TOKEN_LPAREN ||
            current_token.type == TOKEN_RPAREN ||
            current_token.type == TOKEN_EQEQ   ||
            current_token.type == TOKEN_NEQ    ||
            current_token.type == TOKEN_LT     ||
            current_token.type == TOKEN_GT     ||
            current_token.type == TOKEN_LTE    ||
            current_token.type == TOKEN_GTE    ||
            current_token.type == TOKEN_AND    ||
            current_token.type == TOKEN_OR     ||
            current_token.type == TOKEN_NOT) {

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
    stmt.ret_stmt.expr = nullptr;

    // Consume 'ret' token
    eat(TOKEN_RETURN);

    // Check if there's an expression before semicolon
    if (current_token.type != TOKEN_SEMICOLON) {
        stmt.ret_stmt.expr = parse_expression();
    }
    // Expect semicolon
    eat(TOKEN_SEMICOLON);
    return stmt;
}
static Statement parse_let_statement() {
    Statement stmt;
    stmt.type = STMT_LET;
    stmt.let_stmt.expr = nullptr;
    eat(TOKEN_LET);
    stmt.let_stmt.ident = strdup(current_token.value);
    eat(TOKEN_IDENT);
    eat(TOKEN_EQ);

    // Parse expression
    stmt.let_stmt.expr = parse_expression();

    eat(TOKEN_SEMICOLON);
    return stmt;
}
static Program parser_parse_block() {
    Program block;
    block.count = 0;
    block.capacity = 10;
    block.statements = malloc(block.capacity * sizeof(Statement));

    while (current_token.type != TOKEN_RBRACE && current_token.type != TOKEN_EOF) {
        Statement stmt;

        switch (current_token.type) {
            case TOKEN_RETURN:
                stmt = parse_return_statement();
                break;
            case TOKEN_LET:
                stmt = parse_let_statement();
                break;
            case TOKEN_IF:
                stmt = parse_if_statement();
                break;
            default:
                fprintf(stderr, "Syntax error: Unexpected token %s at line %d, column %d\n",
                        token_type_to_string(current_token.type),
                        current_token.line,
                        current_token.column);
                exit(EXIT_FAILURE);
        }

        if (block.count >= block.capacity) {
            block.capacity *= 2;
            Statement* tmp = realloc(block.statements, block.capacity * sizeof(Statement));
            if (tmp == NULL) {
                free(block.statements);
                fprintf(stderr, "Memory allocation error\n");
                exit(EXIT_FAILURE);
            }
            block.statements = tmp;
        }

        block.statements[block.count++] = stmt;
    }

    eat(TOKEN_RBRACE);
    return block;
}
static Statement parse_if_statement() {
    Statement stmt;
    stmt.type = STMT_IF;
    stmt.if_stmt.condition = nullptr;
    stmt.if_stmt.if_block = nullptr;
    stmt.if_stmt.if_count = 0;
    stmt.if_stmt.else_block = nullptr;
    stmt.if_stmt.else_count = 0;

    // Consume 'if' token
    eat(TOKEN_IF);

    // Parse condition
    stmt.if_stmt.condition = parse_expression();

    // Parse if block
    eat(TOKEN_LBRACE);

    // Parse statements in if block
    const Program if_block = parser_parse_block();
    stmt.if_stmt.if_block = if_block.statements;
    stmt.if_stmt.if_count = if_block.count;

    // Check for else block
    if (current_token.type == TOKEN_ELSE) {
        eat(TOKEN_ELSE);
        eat(TOKEN_LBRACE);

        // Parse statements in else block
        const Program else_block = parser_parse_block();
        stmt.if_stmt.else_block = else_block.statements;
        stmt.if_stmt.else_count = else_block.count;
    }

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
            case TOKEN_IF:
                stmt = parse_if_statement();
                break;
            default:
                fprintf(stderr, "Syntax error: Unexpected token %s at line %d, column %d\n",
                        token_type_to_string(current_token.type),
                        current_token.line,
                        current_token.column);
                exit(EXIT_FAILURE);
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
    for (int i = 0; i < program->count; i++) {
        switch (program->statements[i].type) {
            case STMT_RETURN:
                if (program->statements[i].ret_stmt.expr)
                    expression_free(program->statements[i].ret_stmt.expr);
                break;
            case STMT_LET:
                if (program->statements[i].let_stmt.ident)
                    free(program->statements[i].let_stmt.ident);
                if (program->statements[i].let_stmt.expr)
                    expression_free(program->statements[i].let_stmt.expr);
                break;
            case STMT_IF:
                if_statement_free(&program->statements[i].if_stmt);
                break;
        }
    }

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
void if_statement_free(const IfStatement* if_stmt) {
    if (if_stmt->condition) {
        expression_free(if_stmt->condition);
    }

    // Free if block statements
    for (int i = 0; i < if_stmt->if_count; i++) {
        switch (if_stmt->if_block[i].type) {
            case STMT_RETURN:
                if (if_stmt->if_block[i].ret_stmt.expr)
                    expression_free(if_stmt->if_block[i].ret_stmt.expr);
                break;
            case STMT_LET:
                if (if_stmt->if_block[i].let_stmt.ident)
                    free(if_stmt->if_block[i].let_stmt.ident);
                if (if_stmt->if_block[i].let_stmt.expr)
                    expression_free(if_stmt->if_block[i].let_stmt.expr);
                break;
            case STMT_IF:
                if_statement_free(&if_stmt->if_block[i].if_stmt);
                break;
        }
    }
    free(if_stmt->if_block);

    // Free else block statements - same changes as above
    for (int i = 0; i < if_stmt->else_count; i++) {
        switch (if_stmt->else_block[i].type) {
            case STMT_RETURN:
                if (if_stmt->else_block[i].ret_stmt.expr)
                    expression_free(if_stmt->else_block[i].ret_stmt.expr);
                break;
            case STMT_LET:
                if (if_stmt->else_block[i].let_stmt.ident)
                    free(if_stmt->else_block[i].let_stmt.ident);
                if (if_stmt->else_block[i].let_stmt.expr)
                    expression_free(if_stmt->else_block[i].let_stmt.expr);
                break;
            case STMT_IF:
                if_statement_free(&if_stmt->else_block[i].if_stmt);
                break;
        }
    }
    free(if_stmt->else_block);
}