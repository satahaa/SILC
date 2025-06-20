#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

int main(const int argc, const char** argv) {

    const char* c_file = "a.c";
    const char* exe_file = "a.exe";

    if (argc < 2) {
        fprintf(stderr, "Usage: %s <input_file.cor>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    const char* input_file = argv[1];
    if (strstr(input_file, ".cor") == NULL) {
        fprintf(stderr, "Error: Input file must have a .cor extension\n");
        exit(EXIT_FAILURE);
    }

    if (argc > 2) {
        exe_file = argv[2];
    }

    // Check if input file exists
    FILE* source = fopen(input_file, "r");
    if (source == NULL) {
        fprintf(stderr, "Error: Could not open input file %s\n", input_file);
        exit(EXIT_FAILURE);
    }

    // Initialize the compiler components
    lexer_init(source);
    parser_init();
    codegen_init(c_file);

    // Parse the input
    Program program = parser_parse();

    //Generate code
    codegen_generate(program);
    codegen_cleanup();
    program_free(&program);
    parser_cleanup();
    fclose(source);
    // Check if GCC is installed
    if (system("gcc --version > nul 2>&1") != 0) {
        fprintf(stderr, "GCC is not installed! Aborting..\n");
        exit(EXIT_FAILURE);
    }
    // Compile code
    char command[256];
    sprintf(command, "gcc %s -o %s", c_file, exe_file);
    const int ret = system(command);

    if (ret) {
        fprintf(stderr, "Compilation failed! Aborting.. \n");
        exit(EXIT_FAILURE);
    }
    // Clean up intermediate files
    //remove(c_file);

    printf("Compilation completed successfully. Executable created: %s\n", exe_file);

    return 0;
}