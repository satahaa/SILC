#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/lexer.h"

// Helper function to create a file with test content
FILE* create_test_file(const char* content) {
    FILE* file = tmpfile();
    if (file) {
        fputs(content, file);
        rewind(file);
    }
    return file;
}

void test_lexer_init_cleanup() {
    FILE* test_file = create_test_file("let x = 5;");
    lexer_init(test_file);
    lexer_cleanup();
    printf("PASS: lexer_init_cleanup\n");
}

void test_identifier_token() {
    FILE* test_file = create_test_file("let variable = 42;");
    lexer_init(test_file);

    // First token (let)
    Token token = {TOKEN_UNKNOWN, NULL, 0, 0};
    token = lexer_next_token(token);
    assert(token.type == TOKEN_LET);

    // Second token (variable)
    token = lexer_next_token(token);
    assert(token.type == TOKEN_IDENT);
    assert(strcmp(token.value, "variable") == 0);

    lexer_cleanup();
    printf("PASS: test_identifier_token\n");
}

void test_integer_token() {
    FILE* test_file = create_test_file("42");
    lexer_init(test_file);

    Token token = {TOKEN_UNKNOWN, NULL, 0, 0};
    token = lexer_next_token(token);
    assert(token.type == TOKEN_NUMBER);
    assert(atoi(token.value) == 42);

    lexer_cleanup();
    printf("PASS: test_integer_token\n");
}

void test_return_token() {
    FILE* test_file = create_test_file("return 10;");
    lexer_init(test_file);

    Token token = {TOKEN_UNKNOWN, NULL, 0, 0};
    token = lexer_next_token(token);
    assert(token.type == TOKEN_RETURN);

    lexer_cleanup();
    printf("PASS: test_return_token\n");
}

int main() {
    printf("Running lexer tests...\n");

    test_lexer_init_cleanup();
    test_identifier_token();
    test_integer_token();
    test_return_token();

    printf("All lexer tests passed!\n");
    return 0;
}