#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "parser.h"

static Token current_token;
static bool is_in_loop = false;

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
           current_token.type != TOKEN_RBRACE    &&
           current_token.type != TOKEN_COLON     &&
           current_token.type != TOKEN_LBRACE) {

        if (current_token.type == TOKEN_NUMBER ||
            current_token.type == TOKEN_IDENT  ||
            current_token.type == TOKEN_PLUS   ||
            current_token.type == TOKEN_MINUS  ||
            current_token.type == TOKEN_MUL    ||
            current_token.type == TOKEN_DIV    ||
            current_token.type == TOKEN_LPAREN ||
            current_token.type == TOKEN_RPAREN ||
            current_token.type == TOKEN_MOD    ||
            current_token.type == TOKEN_EQ     ||
            current_token.type == TOKEN_EQEQ   ||
            current_token.type == TOKEN_NEQ    ||
            current_token.type == TOKEN_LT     ||
            current_token.type == TOKEN_GT     ||
            current_token.type == TOKEN_LTE    ||
            current_token.type == TOKEN_GTE    ||
            current_token.type == TOKEN_AND    ||
            current_token.type == TOKEN_OR     ||
            current_token.type == TOKEN_NOT    ||
            current_token.type == TOKEN_STRING ||
            current_token.type == TOKEN_XOR    ||
            current_token.type == TOKEN_BITWISE_OR ||
            current_token.type == TOKEN_BITWISE_AND||
            current_token.type == TOKEN_LSHIFT ||
            current_token.type == TOKEN_RSHIFT ||
            current_token.type == TOKEN_BITWISE_NOT) {

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
            }
        else break;
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


    if (current_token.type != TOKEN_SEMICOLON) {
        // Forbid returning a string literal directly
        if (current_token.type == TOKEN_STRING) {
            fprintf(stderr, "Syntax error: Cannot return a string at line %d, column %d\n",
                    current_token.line, current_token.column);
            exit(EXIT_FAILURE);
        }
        stmt.ret_stmt.expr = parse_expression();
    }
    // Expect semicolon
    eat(TOKEN_SEMICOLON);
    return stmt;
}

static Statement parse_let_statement() {
    Statement stmt;
    stmt.type = STMT_LET;
    stmt.let_stmt.expr = NULL; // Default to no expression
    eat(TOKEN_LET);
    stmt.let_stmt.ident = strdup(current_token.value);
    eat(TOKEN_IDENT);

    // If there is an equals sign, parse the expression
    if (current_token.type == TOKEN_EQ) {
        eat(TOKEN_EQ);
        stmt.let_stmt.expr = parse_expression();
    }

    // A 'let' statement must end with a semicolon
    eat(TOKEN_SEMICOLON);
    return stmt;
}

static Statement parse_while_statement() {
    Statement stmt;
    stmt.type = STMT_WHILE;

    eat(TOKEN_WHILE);

    // Parse the condition expression
    stmt.while_stmt.condition = parse_expression();

    // Set loop context for the body
    const bool previous_loop_state = is_in_loop;
    is_in_loop = true;

    // Parse the body
    eat(TOKEN_LBRACE);
    const Program block = parser_parse_block();
    stmt.while_stmt.body = block.statements;
    stmt.while_stmt.body_count = block.count;
    eat(TOKEN_RBRACE);

    // Restore previous loop context
    is_in_loop = previous_loop_state;

    // No semicolon after while block
    return stmt;
}

static Statement parse_break_statement() {
    Statement stmt;
    stmt.type = STMT_BREAK;
    eat(TOKEN_BREAK);
    eat(TOKEN_SEMICOLON);
    return stmt;
}

static Statement parse_continue_statement() {
    Statement stmt;
    stmt.type = STMT_CONTINUE;
    eat(TOKEN_CONTINUE);
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
            case TOKEN_OUT:
                stmt = parse_out_statement();
                break;
            case TOKEN_IN:
                stmt = parse_in_statement();
                break;
            case TOKEN_BREAK:
                stmt = parse_break_statement();
                break;
            case TOKEN_CONTINUE:
                stmt = parse_continue_statement();
                break;
            case TOKEN_WHILE:
                stmt = parse_while_statement();
                break;
            case TOKEN_IDENT:
            case TOKEN_NUMBER:
            case TOKEN_LPAREN:
                stmt = parse_expression_statement();
                break;
            default:
                fprintf(stderr, "Syntax error: Unexpected token %s in block at line %d, column %d\n",
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

    return block;
}
static Statement parse_if_statement() {
    Statement stmt;
    stmt.type = STMT_IF;

    eat(TOKEN_IF);

    stmt.if_stmt.condition = parse_expression();

    eat(TOKEN_LBRACE);
    const Program true_block = parser_parse_block();
    stmt.if_stmt.if_block = true_block.statements;
    stmt.if_stmt.if_count = true_block.count;
    eat(TOKEN_RBRACE);

    if (current_token.type == TOKEN_ELSE) {
        eat(TOKEN_ELSE);
        eat(TOKEN_LBRACE);
        const Program false_block = parser_parse_block();
        stmt.if_stmt.else_block = false_block.statements;
        stmt.if_stmt.else_count = false_block.count;
        eat(TOKEN_RBRACE);
    } else {
        stmt.if_stmt.else_block = NULL;
        stmt.if_stmt.else_count = 0;
    }

    // No semicolon after if-else block
    return stmt;
}

static Statement parse_out_statement() {
    Statement stmt;
    stmt.type = STMT_OUT;

    // Consume 'out' token
    eat(TOKEN_OUT);

    // Parse the expression to be printed
    stmt.out_stmt.expr = parse_expression();

    // Expect semicolon
    eat(TOKEN_SEMICOLON);
    return stmt;
}

static Statement parse_in_statement() {
    Statement stmt;
    stmt.type = STMT_IN;

    eat(TOKEN_IN);
    stmt.in_stmt.ident = strdup(current_token.value);
    eat(TOKEN_IDENT);
    eat(TOKEN_SEMICOLON);

    return stmt;
}

static Statement parse_expression_statement() {
    Statement stmt;
    stmt.type = STMT_EXPR;
    stmt.expr_stmt.expr = parse_expression();
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
            case TOKEN_IF:
                stmt = parse_if_statement();
                break;
            case TOKEN_OUT:
                stmt = parse_out_statement();
                break;
            case TOKEN_IN:
                stmt = parse_in_statement();
                break;
            case TOKEN_WHILE:
                stmt = parse_while_statement();
                break;
            case TOKEN_BREAK:
                stmt = parse_break_statement();
                break;
            case TOKEN_CONTINUE:
                stmt = parse_continue_statement();
                break;
            case TOKEN_IDENT: // Explicitly handle expression statements starting with an identifier
            case TOKEN_NUMBER:
            case TOKEN_LPAREN:
                stmt = parse_expression_statement();
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
                exit(EXIT_FAILURE);
            }
            program.statements = tmp;
        }

        program.statements[program.count++] = stmt;
    }

    return program;
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
            case STMT_OUT:
                if (program->statements[i].out_stmt.expr)
                    expression_free(program->statements[i].out_stmt.expr);
                break;
            case STMT_EXPR:
                if (program->statements[i].expr_stmt.expr)
                    expression_free(program->statements[i].expr_stmt.expr);
                break;
            case STMT_WHILE:
                while_statement_free(&program->statements[i].while_stmt);
                break;
            default: ;
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
            case STMT_OUT:
                if (if_stmt->if_block[i].out_stmt.expr)
                    expression_free(if_stmt->if_block[i].out_stmt.expr);
                break;
            case STMT_IN:
                if (if_stmt->if_block[i].in_stmt.ident)
                    free(if_stmt->if_block[i].in_stmt.ident);
                break;
            case STMT_EXPR:
                if (if_stmt->if_block[i].expr_stmt.expr)
                    expression_free(if_stmt->if_block[i].expr_stmt.expr);
                break;
            case STMT_WHILE:
                while_statement_free(&if_stmt->if_block[i].while_stmt);
                break;

            default: ;
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
            case STMT_OUT:
                if (if_stmt->else_block[i].out_stmt.expr)
                    expression_free(if_stmt->else_block[i].out_stmt.expr);
                break;
            case STMT_IN:
                if (if_stmt->else_block[i].in_stmt.ident)
                    free(if_stmt->else_block[i].in_stmt.ident);
                break;
            case STMT_EXPR:
                if (if_stmt->else_block[i].expr_stmt.expr)
                    expression_free(if_stmt->else_block[i].expr_stmt.expr);
                break;
            case STMT_WHILE:
                while_statement_free(&if_stmt->else_block[i].while_stmt);
                break;
            default: ;
        }
    }
    free(if_stmt->else_block);
}
void while_statement_free(const WhileStatement* while_stmt) {
    if (while_stmt->condition) {
        expression_free(while_stmt->condition);
    }

    // Free body statements
    for (int i = 0; i < while_stmt->body_count; i++) {
        switch (while_stmt->body[i].type) {
            case STMT_RETURN:
                if (while_stmt->body[i].ret_stmt.expr)
                    expression_free(while_stmt->body[i].ret_stmt.expr);
                break;
            case STMT_LET:
                if (while_stmt->body[i].let_stmt.ident)
                    free(while_stmt->body[i].let_stmt.ident);
                if (while_stmt->body[i].let_stmt.expr)
                    expression_free(while_stmt->body[i].let_stmt.expr);
                break;
            case STMT_IF:
                if_statement_free(&while_stmt->body[i].if_stmt);
                break;
            case STMT_OUT:
                if (while_stmt->body[i].out_stmt.expr)
                    expression_free(while_stmt->body[i].out_stmt.expr);
                break;
            case STMT_IN:
                if (while_stmt->body[i].in_stmt.ident)
                    free(while_stmt->body[i].in_stmt.ident);
                break;
            case STMT_EXPR:
                if (while_stmt->body[i].expr_stmt.expr)
                    expression_free(while_stmt->body[i].expr_stmt.expr);
                break;
            case STMT_WHILE:
                while_statement_free(&while_stmt->body[i].while_stmt);
                break;

            default: ;
        }
    }
    free(while_stmt->body);
}

void parser_cleanup() {
    token_free(&current_token);
}