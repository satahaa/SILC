#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/parser.h"
#include "../include/lexer.h"

FILE* create_test_file(const char* content) {
    FILE* file = tmpfile();
    if (file) {
        fputs(content, file);
        rewind(file);
    }
    return file;
}

void test_parse_let_statement() {
    FILE* test_file = create_test_file("let x = 42;");
    lexer_init(test_file);
    parser_init();

    Program program = parser_parse();
    assert(program.count == 1);
    assert(program.statements[0].type == STMT_LET);
    assert(strcmp(program.statements[0].let_stmt.ident, "x") == 0);
    assert(program.statements[0].let_stmt.value == 42);

    program_free(&program);
    parser_cleanup();
    lexer_cleanup();
    printf("PASS: test_parse_let_statement\n");
}

void test_parse_return_statement() {
    FILE* test_file = create_test_file("return 42;");
    lexer_init(test_file);
    parser_init();

    Program program = parser_parse();
    assert(program.count == 1);
    assert(program.statements[0].type == STMT_RETURN);
    assert(program.statements[0].ret_stmt.ident == NULL);
    assert(program.statements[0].ret_stmt.value == 42);

    program_free(&program);
    parser_cleanup();
    lexer_cleanup();
    printf("PASS: test_parse_return_statement\n");
}

void test_parse_return_identifier() {
    FILE* test_file = create_test_file("let x = 42; return x;");
    lexer_init(test_file);
    parser_init();

    Program program = parser_parse();
    assert(program.count == 2);
    assert(program.statements[1].type == STMT_RETURN);
    assert(strcmp(program.statements[1].ret_stmt.ident, "x") == 0);

    program_free(&program);
    parser_cleanup();
    lexer_cleanup();
    printf("PASS: test_parse_return_identifier\n");
}

void test_multiple_statements() {
    FILE* test_file = create_test_file("let x = 10; let y = 20; return y;");
    lexer_init(test_file);
    parser_init();

    Program program = parser_parse();
    assert(program.count == 3);

    program_free(&program);
    parser_cleanup();
    lexer_cleanup();
    printf("PASS: test_multiple_statements\n");
}

int main() {
    printf("Running parser tests...\n");

    test_parse_let_statement();
    test_parse_return_statement();
    test_parse_return_identifier();
    test_multiple_statements();

    printf("All parser tests passed!\n");
    return 0;
}