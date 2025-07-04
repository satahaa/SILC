#include <stdio.h>
#include <stdlib.h>
#include "codegen.h"
#include <string.h>
static FILE* output;
static int indent_level = 1;
static int temp_var_counter = 0;
static Symbol symbol_table[1024];
static int symbol_count = 0;

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

// Function to add a variable to the symbol table
static void add_symbol(const char* name, const VarType type) {
    if (symbol_count < 1024) {
        strcpy(symbol_table[symbol_count].name, name);
        symbol_table[symbol_count].type = type;
        symbol_count++;
    }
}

// Function to get a variable's type from the symbol table
static VarType get_symbol_type(const char* name) {
    for (int i = 0; i < symbol_count; i++) {
        if (strcmp(symbol_table[i].name, name) == 0) {
            return symbol_table[i].type;
        }
    }
    return TYPE_DOUBLE; // Default to double if not found
}


void codegen_expression(const Expression* expr) {
    if (!expr || expr->len == 0) {
        fprintf(output, "0");  // Default for empty expressions
        return;
    }

    for (int i = 0; i < expr->len; i++) {
        switch (expr->token_types[i]) {
            case TOKEN_NUMBER:
                if (strchr(expr->token_values[i], '.') == NULL) {
                    fprintf(output, " %s.0", expr->token_values[i]);
                } else {
                    fprintf(output, " %s", expr->token_values[i]);
                }
                break;
            case TOKEN_MOD:
            case TOKEN_XOR:
            case TOKEN_BITWISE_OR:
            case TOKEN_BITWISE_AND:
            case TOKEN_LSHIFT:
            case TOKEN_RSHIFT:
                // Cast left operand to long
                if (i > 0) {
                    if (expr->token_types[i-1] == TOKEN_NUMBER) {
                        const unsigned long seek_len = strlen(expr->token_values[i-1]) + 4; // " " + number + ".0"
                        fseek(output, -seek_len, SEEK_CUR);
                        fprintf(output, " (long)(%s.0)", expr->token_values[i-1]);
                    } else if (expr->token_types[i-1] == TOKEN_IDENT) {
                        const unsigned long seek_len = strlen(expr->token_values[i-1]) + 1; // " " + identifier
                        fseek(output, -seek_len, SEEK_CUR);
                        fprintf(output, " (long)(%s)", expr->token_values[i-1]);
                    }
                }

                // Print the C operator
                if (expr->token_types[i] == TOKEN_MOD) fprintf(output, " %%");
                else if (expr->token_types[i] == TOKEN_XOR) fprintf(output, " ^");
                else if (expr->token_types[i] == TOKEN_BITWISE_OR) fprintf(output, " |");
                else if (expr->token_types[i] == TOKEN_BITWISE_AND) fprintf(output, " &");
                else if (expr->token_types[i] == TOKEN_LSHIFT) fprintf(output, " <<");
                else if (expr->token_types[i] == TOKEN_RSHIFT) fprintf(output, " >>");

                // Cast the right operand and skip it in the next loop iteration
                if (i + 1 < expr->len) {
                     if (expr->token_types[i+1] == TOKEN_NUMBER) {
                        fprintf(output, " (long)(%s.0)", expr->token_values[i+1]);
                     } else if (expr->token_types[i+1] == TOKEN_IDENT) {
                        fprintf(output, " (long)(%s)", expr->token_values[i+1]);
                     }
                    i++; // Manually advance loop counter
                }
                break;
            case TOKEN_BITWISE_NOT:
                fprintf(output, " ~");
                // Cast the right operand and skip it in the next loop iteration
                if (i + 1 < expr->len) {
                    if (expr->token_types[i+1] == TOKEN_NUMBER) {
                        fprintf(output, "(long)(%s.0)", expr->token_values[i+1]);
                    } else if (expr->token_types[i+1] == TOKEN_IDENT) {
                        fprintf(output, "(long)(%s)", expr->token_values[i+1]);
                    }
                    i++; // Manually advance loop counter
                }
                break;
            case TOKEN_STRING:
                fprintf(output, " \"%s\"", expr->token_values[i]);
                break;
            case TOKEN_IDENT:
            case TOKEN_PLUS:
            case TOKEN_MINUS:
            case TOKEN_MUL:
            case TOKEN_DIV:
            case TOKEN_LPAREN:
            case TOKEN_RPAREN:
            case TOKEN_LBRACE:
            case TOKEN_RBRACE:
            case TOKEN_EQ:
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
                VarType type;
                // Check if the expression is a string literal to determine type
                if (stmt.let_stmt.expr != NULL && stmt.let_stmt.expr->len == 1 && stmt.let_stmt.expr->token_types[0] == TOKEN_STRING) {
                    // It's a string initialization
                    fprintf(output, "char %s[256] =", stmt.let_stmt.ident);
                    codegen_expression(stmt.let_stmt.expr);
                    type = TYPE_STRING;
                } else {
                    // It's a double or uninitialized
                    fprintf(output, "double %s", stmt.let_stmt.ident);
                    if (stmt.let_stmt.expr != NULL) {
                        fprintf(output, " =");
                        codegen_expression(stmt.let_stmt.expr);
                    }
                    type = TYPE_DOUBLE;
                }
                fprintf(output, ";\n");
                // Add the new variable to our symbol table
                add_symbol(stmt.let_stmt.ident, type);
                break;

            case STMT_RETURN:
                if (stmt.ret_stmt.expr != NULL) {
                    // Check if the expression is a single identifier that is a string variable
                    if (stmt.ret_stmt.expr->len == 1 && stmt.ret_stmt.expr->token_types[0] == TOKEN_IDENT) {
                        if (get_symbol_type(stmt.ret_stmt.expr->token_values[0]) == TYPE_STRING) {
                            fprintf(stderr, "Error: Cannot return a string variable.\n");
                            exit(EXIT_FAILURE);
                        }
                    }
                    // The parser already prevents returning string literals.
                    // This logic assumes returning complex expressions involving strings is also invalid.
                    fprintf(output, "exit(");
                    codegen_expression(stmt.ret_stmt.expr);
                    fprintf(output, ");\n");
                } else {
                    fprintf(stderr, "Expected expression after return statement\n");
                    exit(EXIT_FAILURE);
                }
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
                bool is_string_var = false;
                bool is_string_literal = false;

                if (stmt.out_stmt.expr != NULL && stmt.out_stmt.expr->len == 1) {
                    if (stmt.out_stmt.expr->token_types[0] == TOKEN_STRING) {
                        is_string_literal = true;
                    } else if (stmt.out_stmt.expr->token_types[0] == TOKEN_IDENT) {
                        if (get_symbol_type(stmt.out_stmt.expr->token_values[0]) == TYPE_STRING) {
                            is_string_var = true;
                        }
                    }
                }

                if (is_string_literal) {
                    // If it's a string literal, print it directly.
                    fprintf(output, "printf(\"%s\");\n", stmt.out_stmt.expr->token_values[0]);
                } else if (is_string_var) {
                    // If it's a string variable, print it using a format specifier.
                    fprintf(output, "printf(\"%%s\\n\", %s);\n", stmt.out_stmt.expr->token_values[0]);
                } else {
                    // Existing logic for numbers and other expressions
                    fprintf(output, "{\n");
                    indent_level++;
                    add_indent();
                    fprintf(output, "double temp_val_%d =", temp_var_counter);
                    codegen_expression(stmt.out_stmt.expr);
                    fprintf(output, ";\n");
                    add_indent();
                    fprintf(output, "if (floor(temp_val_%d) == ceil(temp_val_%d)) {\n", temp_var_counter, temp_var_counter);
                    indent_level++;
                    add_indent();
                    fprintf(output, "printf(\"%%.0f\\n\", temp_val_%d);\n", temp_var_counter);
                    indent_level--;
                    add_indent();
                    fprintf(output, "} else {\n");
                    indent_level++;
                    add_indent();
                    fprintf(output, "printf(\"%%f\\n\", temp_val_%d);\n", temp_var_counter);
                    indent_level--;
                    add_indent();
                    fprintf(output, "}\n");
                    indent_level--;
                    add_indent();
                    fprintf(output, "}\n");
                    temp_var_counter++;
                }
                break;
            case STMT_IN:
                const char* ident = stmt.in_stmt.ident;
                type = get_symbol_type(ident);
                if (type == TYPE_STRING) {
                    fprintf(output, "scanf(\"%%255s\", %s);\n", ident);
                } else {
                    fprintf(output, "scanf(\"%%lf\", &%s);\n", ident);
                }
                break;
            case STMT_WHILE:
                add_indent();
                fprintf(output, "while (");
                codegen_expression(stmt.while_stmt.condition);
                fprintf(output, ") {\n");

                indent_level++;
                codegen_statements(stmt.while_stmt.body, stmt.while_stmt.body_count);
                indent_level--;

                add_indent();
                fprintf(output, "}\n");
                break;
            case STMT_EXPR:
                const Expression* expr = stmt.expr_stmt.expr;

                // Check for string assignment pattern: identifier = "string"
                if (expr->len == 3 &&
                    expr->token_types[0] == TOKEN_IDENT &&
                    expr->token_types[1] == TOKEN_EQ &&
                    expr->token_types[2] == TOKEN_STRING) {

                    const char* identifier = expr->token_values[0];
                    if (get_symbol_type(identifier) == TYPE_STRING) {
                        // Generate strcpy for string assignment
                        fprintf(output, "strcpy(%s, \"%s\");\n", identifier, expr->token_values[2]);
                    } else {
                        // Not a string variable, so generate a standard assignment.
                        // This could be an error (e.g., num_var = "string"), which C would catch.
                        codegen_expression(expr);
                        fprintf(output, ";\n");
                    }
                    } else {
                        // For all other expressions, generate the code as before.
                        codegen_expression(expr);
                        fprintf(output, ";\n");
                    }
                break;
            default: ;
        }
    }
}

void codegen_generate(const Program program) {
    fprintf(output, "#include <stdio.h>\n");
    fprintf(output, "#include <stdlib.h>\n");
    fprintf(output, "#include <string.h>\n");
    fprintf(output, "#include <math.h>\n\n");
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