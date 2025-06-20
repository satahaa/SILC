#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"

static FILE* output;
static int indent_level = 1;

// Helper function to add proper indentation
static void add_indent() {
    for (int i = 0; i < indent_level; i++) {
        fprintf(output, "\t");
    }
}

void codegen_init(const char* output_file) {
    output = fopen(output_file, "w");
    if (output == NULL) {
        fprintf(stderr, "Error: Could not open output file %s\n", output_file);
        exit(EXIT_FAILURE);
    }
}

void codegen_expression(const Expression* expr) {
    if (!expr || expr->len == 0) {
        fprintf(output, "0");  // Default for empty expressions
        return;
    }

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
            case TOKEN_EQEQ:
            case TOKEN_NEQ:
            case TOKEN_LT:
            case TOKEN_GT:
            case TOKEN_LTE:
            case TOKEN_GTE:
                fprintf(output, " %s", expr->token_values[i]);
                break;
            case TOKEN_AND:
                fprintf(output, " &&");
                break;
            case TOKEN_OR:
                fprintf(output, " ||");
                break;
            default:
                fprintf(stderr, "Error: Invalid token in expression\n");
                exit(EXIT_FAILURE);
        }
    }
}

// Generate code for statements in a block
static void codegen_statements(const Statement* statements, const int count) {
    for (int i = 0; i < count; i++) {
        const Statement stmt = statements[i];
        add_indent();

        switch (stmt.type) {
            case STMT_LET:
                fprintf(output, "long long %s =", stmt.let_stmt.ident);
                if (stmt.let_stmt.expr != NULL) {
                    codegen_expression(stmt.let_stmt.expr);
                } else {
                    fprintf(stderr, "Expected expression after let statement\n");
                    exit(EXIT_FAILURE);
                }
                fprintf(output, ";\n");
                break;

            case STMT_RETURN:
                fprintf(output, "exit(");
                if (stmt.ret_stmt.expr != NULL) {
                    codegen_expression(stmt.ret_stmt.expr);
                } else {
                    fprintf(stderr, "Expected expression after return statement\n");
                    exit(EXIT_FAILURE);
                }
                fprintf(output, ");\n");
                break;

            case STMT_IF:
                fprintf(output, "if (");
                codegen_expression(stmt.if_stmt.condition);
                fprintf(output, ") {\n");

                indent_level++;
                codegen_statements(stmt.if_stmt.if_block, stmt.if_stmt.if_count);
                indent_level--;

                add_indent();
                fprintf(output, "}");

                if (stmt.if_stmt.else_count > 0) {
                    fprintf(output, " else {\n");
                    indent_level++;
                    codegen_statements(stmt.if_stmt.else_block, stmt.if_stmt.else_count);
                    indent_level--;
                    add_indent();
                    fprintf(output, "}");
                }
                fprintf(output, "\n");
                break;
            case STMT_OUT:
                fprintf(output, "printf(\"%%lld\\n\", (long long)(");
                codegen_expression(stmt.out_stmt.expr);
                fprintf(output, "));\n");
                break;
            default:
                fprintf(stderr, "Error: Unknown statement type\n");
                exit(EXIT_FAILURE);
        }
    }
}

void codegen_generate(const Program program) {
    fprintf(output, "#include <stdio.h>\n");
    fprintf(output, "#include <stdlib.h>\n");
    fprintf(output, "int main() {\n");

    // Process each statement in the program
    codegen_statements(program.statements, program.count);

    // Default return if none provided
    add_indent();
    fprintf(output, "return 0;\n");
    fprintf(output, "}\n");
}

void codegen_cleanup() {
    if (output != NULL) {
        fclose(output);
        output = nullptr;
    }
}