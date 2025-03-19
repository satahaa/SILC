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
// Generate code for an expression
void codegen_expression(const Expression* expr) {
    if (!expr || expr->len == 0) {
        fprintf(output, "0");  // Default for empty expressions
        return;
    }

    // Output parentheses around the whole expression for safety
    fprintf(output, "(");

    for (int i = 0; i < expr->len; i++) {
        switch (expr->token_types[i]) {
            case TOKEN_NUMBER:
            case TOKEN_IDENT:
            case TOKEN_PLUS:
            case TOKEN_MINUS:
            case TOKEN_MUL:
            case TOKEN_DIV:
            case TOKEN_LPAREN:
            case TOKEN_RPAREN:
                fprintf(output, "%s", expr->token_values[i]);
            break;

            default:
                fprintf(stderr, "Error: Invalid token in expression\n");
                exit(EXIT_FAILURE);
        }
    }

    fprintf(output, ")");
}

void codegen_generate(const Program program) {
    fprintf(output, "#include <stdio.h>\n");
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
                fprintf(output, "   long long %s = ", stmt.let_stmt.ident);
                if (stmt.let_stmt.expr != nullptr) {
                    codegen_expression(stmt.let_stmt.expr);
                } else if (stmt.let_stmt.ident_value == nullptr) {
                    fprintf(output, "%d", stmt.let_stmt.value);
                } else {
                    fprintf(output, "%s", stmt.let_stmt.ident_value);
                }
                fprintf(output, ";\n");
                break;

            case STMT_RETURN:
                fprintf(output, "   exit(");
                if (stmt.ret_stmt.expr != nullptr) {
                    codegen_expression(stmt.ret_stmt.expr);
                } else if (stmt.ret_stmt.ident == nullptr) {
                    fprintf(output, "%d", stmt.ret_stmt.value);
                } else {
                    fprintf(output, "%s", stmt.ret_stmt.ident);
                }
                fprintf(output, ");\n");
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