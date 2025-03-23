#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>

typedef enum {
    TOKEN_RETURN,
    TOKEN_NUMBER,
    TOKEN_SEMICOLON,
    TOKEN_EOF,
    TOKEN_UNKNOWN,
    TOKEN_LET,
    TOKEN_IDENT,
    TOKEN_EQ,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MUL,
    TOKEN_DIV,
    TOKEN_MOD,
    TOKEN_EQEQ,
    TOKEN_NEQ,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_LTE,
    TOKEN_GTE,
    TOKEN_AND,
    TOKEN_OR,
    TOKEN_NOT,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_LBRACE,
    TOKEN_RBRACE
} Ttype;

typedef struct {
    Ttype type;
    char* value;
    int line;
    int column;
} Token;

// Initialize the lexer with a file
void lexer_init(FILE* source_file);

// Get the next token from the source
Token lexer_next_token();

// Free resources used by the lexer
void lexer_cleanup();

// Utility functions
const char* token_type_to_string(Ttype type);
void token_free(Token* token);

#endif // LEXER_H