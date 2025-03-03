#include "codegen.h"

static FILE* output;

void codegen_init(const char* output_file) {
    output = fopen(output_file, "w");
    if (output == NULL) {
        fprintf(stderr, "Error: Could not open output file %s\n", output_file);
        exit(1);
    }
}

void codegen_generate(Program program) {
    // Write the assembly header for Windows x64
    fprintf(output, "; Generated assembly code for Windows x64\n");
    fprintf(output, "section .text\n");
    fprintf(output, "   global main\n");
    fprintf(output, "   extern ExitProcess\n\n");
    fprintf(output, "main:\n");

    // Process each statement in the program
    for (int i = 0; i < program.count; i++) {
        Statement stmt = program.statements[i];

        switch (stmt.type) {
            case STMT_RETURN:
                // For Windows x64
                fprintf(output, "    ; Return statement\n");

                fprintf(output, "    mov eax, %d     ; return value\n", stmt.ret_stmt.value);
                fprintf(output, "    call ExitProcess\n");
                //fprintf(output, "    ret\n");
                break;
        }
    }
}

void codegen_cleanup() {
    if (output != NULL) {
        fclose(output);
        output = nullptr;
    }
}