#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "semantic.h"

static ScopeStack scope_stack;
static int in_loop_depth = 0;

static void push_scope();
static void pop_scope();
static SymbolEntry* find_symbol(const char* name);
static SemanticResult add_symbol(const char* name, const VarType type);
static VarType get_expression_type(const Expression* expr);
static SemanticResult analyze_expression(Expression* expr);
static SemanticResult analyze_statement(Statement* stmt);

void semantic_init() {
    scope_stack.scope_capacity = 10;
    scope_stack.scope_count = 0;
    scope_stack.scopes = malloc(sizeof(Scope) * scope_stack.scope_capacity);
    in_loop_depth = 0;

    // Create global scope
    push_scope();
}

void semantic_cleanup() {
    // Pop all scopes
    while (scope_stack.scope_count > 0) {
        pop_scope();
    }

    if (scope_stack.scopes) {
        free(scope_stack.scopes);
        scope_stack.scopes = NULL;
    }
    scope_stack.scope_capacity = 0;
}

static void push_scope() {
    if (scope_stack.scope_count >= scope_stack.scope_capacity) {
        scope_stack.scope_capacity *= 2;
        scope_stack.scopes = realloc(scope_stack.scopes,
                                   sizeof(Scope) * scope_stack.scope_capacity);
        if (!scope_stack.scopes) {
            fprintf(stderr, "Memory allocation error in push_scope\n");
            exit(EXIT_FAILURE);
        }
    }

    Scope* new_scope = &scope_stack.scopes[scope_stack.scope_count];
    new_scope->capacity = 100;
    new_scope->count = 0;
    new_scope->entries = malloc(sizeof(SymbolEntry) * new_scope->capacity);
    if (!new_scope->entries) {
        fprintf(stderr, "Memory allocation error in push_scope\n");
        exit(EXIT_FAILURE);
    }

    scope_stack.scope_count++;
}

static void pop_scope() {
    if (scope_stack.scope_count > 0) {
        scope_stack.scope_count--;
        Scope* scope = &scope_stack.scopes[scope_stack.scope_count];

        if (scope->entries) {
            free(scope->entries);
            scope->entries = NULL;
        }
        scope->count = 0;
        scope->capacity = 0;
    }
}

static SymbolEntry* find_symbol(const char* name) {
    // Search from current scope back to global scope
    for (int scope_idx = scope_stack.scope_count - 1; scope_idx >= 0; scope_idx--) {
        const Scope* scope = &scope_stack.scopes[scope_idx];
        for (int i = 0; i < scope->count; i++) {
            if (strcmp(scope->entries[i].name, name) == 0) {
                return &scope->entries[i];
            }
        }
    }
    return NULL;
}

static SemanticResult add_symbol(const char* name, const VarType type) {
    if (scope_stack.scope_count == 0) {
        push_scope(); // Create global scope if none exists
    }

    Scope* current_scope = &scope_stack.scopes[scope_stack.scope_count - 1];

    // Check if variable already exists in current scope only
    for (int i = 0; i < current_scope->count; i++) {
        if (strcmp(current_scope->entries[i].name, name) == 0) {
            fprintf(stderr, "Semantic Error: Variable '%s' already declared in current scope\n", name);
            return SEMANTIC_ERROR_REDECLARED_VAR;
        }
    }

    if (current_scope->count >= current_scope->capacity) {
        current_scope->capacity *= 2;
        current_scope->entries = realloc(current_scope->entries,
                                       sizeof(SymbolEntry) * current_scope->capacity);
        if (!current_scope->entries) {
            fprintf(stderr, "Memory allocation error in add_symbol\n");
            exit(EXIT_FAILURE);
        }
    }

    strncpy(current_scope->entries[current_scope->count].name, name, 255);
    current_scope->entries[current_scope->count].name[255] = '\0';
    current_scope->entries[current_scope->count].type = type;
    current_scope->entries[current_scope->count].is_declared = 1;
    current_scope->count++;

    return SEMANTIC_OK;
}

static VarType get_expression_type(const Expression* expr) {
    if (expr->len == 0) return TYPE_DOUBLE;

    // Simple type inference based on first token
    if (expr->token_types[0] == TOKEN_NUMBER) {
        return TYPE_DOUBLE;
    }
    if (expr->token_types[0] == TOKEN_STRING) {
        return TYPE_STRING;
    }
    if (expr->token_types[0] == TOKEN_IDENT) {
        const SymbolEntry* symbol = find_symbol(expr->token_values[0]);
        if (symbol) {
            return symbol->type;
        }
        return TYPE_DOUBLE; // Default fallback
    }

    return TYPE_DOUBLE;
}

static SemanticResult analyze_expression(Expression* expr) {
    if (!expr) return SEMANTIC_OK;

    for (int i = 0; i < expr->len; i++) {
        if (expr->token_types[i] == TOKEN_IDENT) {
            SymbolEntry* symbol = find_symbol(expr->token_values[i]);
            if (!symbol) {
                fprintf(stderr, "Semantic Error: Undeclared variable '%s'\n",
                       expr->token_values[i]);
                return SEMANTIC_ERROR_UNDECLARED_VAR;
            }
        }
    }

    return SEMANTIC_OK;
}

static SemanticResult analyze_statement(Statement* stmt) {
    if (!stmt) return SEMANTIC_OK;

    switch (stmt->type) {
        case STMT_LET: {
            const LetStatement* let_stmt = &stmt->let_stmt;
            SemanticResult result = SEMANTIC_OK;

            if (let_stmt->expr) {
                result = analyze_expression(let_stmt->expr);
                if (result != SEMANTIC_OK) return result;
            }

            const VarType var_type = let_stmt->expr ? get_expression_type(let_stmt->expr) : TYPE_DOUBLE;
            return add_symbol(let_stmt->ident, var_type);
        }

        case STMT_IF: {
            const IfStatement* if_stmt = &stmt->if_stmt;
            SemanticResult result = analyze_expression(if_stmt->condition);
            if (result != SEMANTIC_OK) return result;

            // Push scope for if block
            push_scope();
            for (int i = 0; i < if_stmt->if_count; i++) {
                result = analyze_statement(&if_stmt->if_block[i]);
                if (result != SEMANTIC_OK) {
                    pop_scope();
                    return result;
                }
            }
            pop_scope();

            // Push scope for else block if present
            if (if_stmt->else_block) {
                push_scope();
                for (int i = 0; i < if_stmt->else_count; i++) {
                    result = analyze_statement(&if_stmt->else_block[i]);
                    if (result != SEMANTIC_OK) {
                        pop_scope();
                        return result;
                    }
                }
                pop_scope();
            }

            return SEMANTIC_OK;
        }

        case STMT_WHILE: {
            const WhileStatement* while_stmt = &stmt->while_stmt;
            SemanticResult result = analyze_expression(while_stmt->condition);
            if (result != SEMANTIC_OK) return result;

            in_loop_depth++;
            push_scope(); // Push scope for while body

            for (int i = 0; i < while_stmt->body_count; i++) {
                result = analyze_statement(&while_stmt->body[i]);
                if (result != SEMANTIC_OK) {
                    pop_scope();
                    in_loop_depth--;
                    return result;
                }
            }

            pop_scope(); // Pop scope for while body
            in_loop_depth--;
            return SEMANTIC_OK;
        }

        case STMT_EXPR:
            return analyze_expression(stmt->expr_stmt.expr);
        case STMT_OUT:
            return analyze_expression(stmt->out_stmt.expr);
        case STMT_IN: {
            SymbolEntry* symbol = find_symbol(stmt->in_stmt.ident);
            if (!symbol) {
                fprintf(stderr, "Semantic Error: Undeclared variable '%s'\n", stmt->in_stmt.ident);
                return SEMANTIC_ERROR_UNDECLARED_VAR;
            }
            return SEMANTIC_OK;
        }
        case STMT_BREAK: {
            if (in_loop_depth == 0) {
                fprintf(stderr, "Semantic Error: 'brk' statement outside loop\n");
                return SEMANTIC_ERROR_BREAK_OUTSIDE_LOOP;
            }
            return SEMANTIC_OK;
        }
        case STMT_CONTINUE: {
            if (in_loop_depth == 0) {
                fprintf(stderr, "Semantic Error: 'con' statement outside loop\n");
                return SEMANTIC_ERROR_CONTINUE_OUTSIDE_LOOP;
            }
            return SEMANTIC_OK;
        }
        case STMT_RETURN:
            return stmt->ret_stmt.expr ? analyze_expression(stmt->ret_stmt.expr) : SEMANTIC_OK;
        default:
            return SEMANTIC_OK;
    }
}

SemanticResult semantic_analyze(Program* program) {
    if (!program) return SEMANTIC_OK;

    for (int i = 0; i < program->count; i++) {
        const SemanticResult result = analyze_statement(&program->statements[i]);
        if (result != SEMANTIC_OK) {
            return result;
        }
    }

    return SEMANTIC_OK;
}