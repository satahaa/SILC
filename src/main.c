#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"
#include "semantic.h"
void print_version() {
    printf("SILC v1.2.1\n");
    printf("A Simple Imperative Language Compiler.\n");
    printf("Copyright (C) 2025 Sabah Alam Tahaa.\nThis is free software see the source for copying conditions.\nThere is NO warranty; not even for MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.\n");
}

void print_help() {
    printf("Usage: SILC <file> | [options]\n\n");
    printf("A Simple Imperative Language Compiler.\n\n");
    printf("Options:\n");
    printf("  -v, --version    Print compiler version and exit.\n");
    printf("  -h, --help       Print this help message and exit.\n\n");
    printf("To compile a file:\n");
    printf("  SILC path/to/your/file.slc\n");
}

int main(const int argc, const char** argv) {
    if (argc < 2) {
        fprintf(stderr, "Error: No input file provided. Use 'SILC -h' for help.\n");
        return 1;
    }

    const char* arg = argv[1];

    if (strcmp(arg, "-v") == 0 || strcmp(arg, "--version") == 0) {
        print_version();
        return 0;
    }

    if (strcmp(arg, "-h") == 0 || strcmp(arg, "--help") == 0) {
        print_help();
        return 0;
    }

    // After checking for flags, the first argument must be the input file.
    const char* input_file = arg;
    if (strstr(input_file, ".slc") == NULL) {
        fprintf(stderr, "Error: Input file must have a .slc extension. Got: %s\n", input_file);
        exit(EXIT_FAILURE);
    }

    // Check if input file exists
    FILE* source = fopen(input_file, "r");
    if (source == NULL) {
        fprintf(stderr, "Error: Could not open input file %s\n", input_file);
        exit(EXIT_FAILURE);
    }

    // Check if GCC is installed before proceeding
    if (system("gcc --version > nul 2>&1") != 0) {
        fprintf(stderr, "Error: GCC is not installed or not in the system's PATH. Aborting.\n");
        fclose(source);
        exit(EXIT_FAILURE);
    }

    const char* c_file = "a.c";
    const char* exe_file = "a.exe"; // Default output name

    if (argc > 2) {
        exe_file = argv[2];
    }

    // Initialize the compiler components
    lexer_init(source);
    parser_init();
    semantic_init();  // Add this line
    codegen_init(c_file);

    // Parse the input
    Program program = parser_parse();

    // Perform semantic analysis
    SemanticResult semantic_result = semantic_analyze(&program);
    if (semantic_result != SEMANTIC_OK) {
        fprintf(stderr, "Semantic analysis failed. Compilation aborted.\n");
        semantic_cleanup();
        codegen_cleanup();
        program_free(&program);
        parser_cleanup();
        fclose(source);
        exit(EXIT_FAILURE);
    }

    // Generate C code
    codegen_generate(program);

    // Cleanup compiler components
    semantic_cleanup();  // Add this line
    codegen_cleanup();
    program_free(&program);
    parser_cleanup();
    fclose(source);

    // Compile the generated C code
    char command[256];
    sprintf(command, "gcc %s -o %s", c_file, exe_file);
    const int ret = system(command);

    if (ret != 0) {
        fprintf(stderr, "Error: C compilation failed. Aborting.\n");
        // Keep a.c for debugging
        exit(EXIT_FAILURE);
    }

    // Clean up intermediate C file
    remove(c_file);

    printf("Compilation completed successfully. Executable created: %s\n", exe_file);

    return 0;
}
