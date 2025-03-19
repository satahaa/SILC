#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "../include/codegen.h"
#include "../include/parser.h"

// Helper to check if a file contains expected content
int file_contains(const char* filename, const char* expected_content) {
    FILE* file = fopen(filename, "r");
    if (!file) return 0;

    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    rewind(file);

    char* buffer = malloc(size + 1);
    if (!buffer) {
        fclose(file);
        return 0;
    }

    size_t read_size = fread(buffer, 1, size, file);
    buffer[read_size] = '\0';
    fclose(file);

    int result = strstr(buffer, expected_content) != NULL;
    free(buffer);
    return result;
}

// Helper to create a simple program
Program create_test_program() {
    Program program;
    program.count = 2;
    program.statements = malloc(sizeof(Statement) * program.count);

    // Add let statement
    program.statements[0].type = STMT_LET;
    program.statements[0].let_stmt.ident = strdup("x");
    program.statements[0].let_stmt.value = 42;

    // Add return statement
    program.statements[1].type = STMT_RETURN;
    program.statements[1].ret_stmt.ident = NULL;
    program.statements[1].ret_stmt.value = 42;

    return program;
}

void test_codegen_init_cleanup() {
    const char* test_file = "test_output.c";
    codegen_init(test_file);
    codegen_cleanup();

    // Test that the file was created
    FILE* file = fopen(test_file, "r");
    assert(file != NULL);
    fclose(file);
    remove(test_file);

    printf("PASS: test_codegen_init_cleanup\n");
}

void test_codegen_let_statement() {
    const char* test_file = "test_let.c";
    codegen_init(test_file);

    Program program;
    program.count = 1;
    program.statements = malloc(sizeof(Statement));
    program.statements[0].type = STMT_LET;
    program.statements[0].let_stmt.ident = strdup("test_var");
    program.statements[0].let_stmt.value = 123;

    codegen_generate(program);
    codegen_cleanup();

    assert(file_contains(test_file, "int test_var = 123;"));

    // Cleanup
    free(program.statements[0].let_stmt.ident);
    free(program.statements);
    remove(test_file);

    printf("PASS: test_codegen_let_statement\n");
}

void test_codegen_return_statement() {
    const char* test_file = "test_return.c";
    codegen_init(test_file);

    Program program;
    program.count = 1;
    program.statements = malloc(sizeof(Statement));
    program.statements[0].type = STMT_RETURN;
    program.statements[0].ret_stmt.ident = NULL;
    program.statements[0].ret_stmt.value = 42;

    codegen_generate(program);
    codegen_cleanup();

    assert(file_contains(test_file, "exit(42);"));

    // Cleanup
    free(program.statements);
    remove(test_file);

    printf("PASS: test_codegen_return_statement\n");
}

void test_full_program_generation() {
    const char* test_file = "test_program.c";
    codegen_init(test_file);

    Program program = create_test_program();
    codegen_generate(program);
    codegen_cleanup();

    assert(file_contains(test_file, "int x = 42;"));
    assert(file_contains(test_file, "exit(42);"));

    // Cleanup
    free(program.statements[0].let_stmt.ident);
    free(program.statements);
    remove(test_file);

    printf("PASS: test_full_program_generation\n");
}

int main() {
    printf("Running codegen tests...\n");

    test_codegen_init_cleanup();
    test_codegen_let_statement();
    test_codegen_return_statement();
    test_full_program_generation();

    printf("All codegen tests passed!\n");
    return 0;
}