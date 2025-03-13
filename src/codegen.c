#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"

static FILE* output;

void codegen_init(const char* output_file) {
    output = fopen(output_file, "w");
    if (output == NULL) {
        fprintf(stderr, "Error: Could not open output file %s\n", output_file);
        exit(EXIT_FAILURE);
    }
}

void codegen_generate(const Program program) {
    fprintf(output, "#include <stdio.h>\n");
    fprintf(output, "#include <stdlib.h>\n");
    fprintf(output, "#include <stdlib.h>\n");
    fprintf(output, "#include <ctype.h>\n");
    fprintf(output, "#include <string.h>\n");
    fprintf(output, "#include <stdbool.h>\n");
    fprintf(output, "int main() {\n");
    // Process each statement in the program
    for (int i = 0; i < program.count; i++) {
        const Statement stmt = program.statements[i];

        switch (stmt.type) {
            case STMT_LET:
                fprintf(output, "   int %s = %d;\n", stmt.let_stmt.ident, stmt.let_stmt.value);
                break;
            case STMT_RETURN:
                    fprintf(output, "   return %d;\n", stmt.ret_stmt.value);
                    break;
            default:
                fprintf(stderr, "Error: Unknown statement type\n");
                exit(EXIT_FAILURE);
        }
    }
    fprintf(output, "}\n");
}

void codegen_cleanup() {
    if (output != NULL) {
        fclose(output);
        output = nullptr;
    }
}