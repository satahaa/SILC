#include "parser.h"
#include "lexer.h"

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

static Statement parse_return_statement() {
    Statement stmt;
    stmt.type = STMT_RETURN;

    eat(TOKEN_RETURN);

    if (current_token.type == TOKEN_NUMBER) {
        stmt.ret_stmt.value = atoi(current_token.value);
        eat(TOKEN_NUMBER);
    } else {
        fprintf(stderr, "Syntax error: Expected number after 'ret' at line %d, column %d\n",
                current_token.line, current_token.column);
        exit(1);
    }

    eat(TOKEN_SEMICOLON);

    return stmt;
}
static Statement parse_let_statement() {
    Statement stmt;
    stmt.type = STMT_LET;

    eat(TOKEN_LET);

    if (current_token.type == TOKEN_IDENT) {
        stmt.let_stmt.ident = current_token.value;
        eat(TOKEN_IDENT);
    } else {
        fprintf(stderr, "Syntax error: Expected identifier after 'let' at line %d, column %d\n",
                current_token.line, current_token.column);
        exit(1);
    }

    eat(TOKEN_EQ);

    if (current_token.type == TOKEN_NUMBER) {
        stmt.let_stmt.value = atoi(current_token.value);
        eat(TOKEN_NUMBER);
    } else {
        fprintf(stderr, "Syntax error: Expected number after '=' at line %d, column %d\n",
                current_token.line, current_token.column);
        exit(1);
    }

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
            program.statements = realloc(program.statements, program.capacity * sizeof(Statement));
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