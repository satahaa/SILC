#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

int main(int argc, char** argv) {

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
        return 1;
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
    lexer_cleanup();
    fclose(source);

    // Compile code
    char command[256];
    sprintf(command, "E:\\msys2\\mingw64\\bin\\gcc.exe %s -o %s -Wl,-subsystem,console", c_file, exe_file);
    int ret = system(command);
    printf("Executing command: \"%s\"\n", command);
    if (ret) {
        printf("Compilation failed! Aborting.. \n");
        exit(EXIT_FAILURE);
    }
    // Optional: Clean up intermediate files
    remove(c_file);

    printf("Compilation completed successfully. Executable created: %s\n", exe_file);

    return 0;
}