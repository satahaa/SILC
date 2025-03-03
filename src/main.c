#include <stdio.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

int main() {
    const char* input_file = "../tests/test.cor";
    const char* output_file = "output.asm";

    // Open the input file
    FILE* source = fopen(input_file, "r");
    if (source == NULL) {
        fprintf(stderr, "Error: Could not open input file %s\n", input_file);
        return 1;
    }

    // Initialize the compiler components
    lexer_init(source);
    parser_init();
    codegen_init(output_file);

    // Parse the input
    Program program = parser_parse();

    // Generate code
    codegen_generate(program);

    // Clean up resources
    program_free(&program);
    parser_cleanup();
    lexer_cleanup();
    codegen_cleanup();

    fclose(source);

    printf("Compilation completed successfully. Output written to %s\n", output_file);

    return 0;
}