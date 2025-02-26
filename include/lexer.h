#ifndef LEXER_H
#define LEXER_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

/**
 * Token types with descriptive names
 */
typedef enum {
    TOKEN_LET,
    TOKEN_RET,
    TOKEN_INT,
    TOKEN_IDENTIFIER,
    TOKEN_ASSIGN,
    TOKEN_SEMICOLON,
    TOKEN_EOF
} TokenType;

/**
 * Token structure to represent lexical tokens
 */
typedef struct {
    TokenType type;
    char *value;
    size_t line;
    size_t column;
} Token;

/**
 * Lexer structure to maintain lexer state
 */
typedef struct {
    char *source;
    size_t source_length;
    size_t position;
    size_t line;
    size_t column;
    Token *tokens;
    size_t tokens_capacity;
    size_t tokens_count;
} Lexer;

/**
 * Keyword mapping structure
 */
typedef struct {
    const char *keyword;
    TokenType type;
} Keyword;

/**
 * Initialize a new lexer with the given source
 *
 * @param source The source code to tokenize
 * @param length The length of the source code
 * @return A pointer to the initialized lexer
 */
Lexer* lexer_init(const char *source, size_t length);

/**
 * Free all resources associated with the lexer
 *
 * @param lexer The lexer to free
 */
void lexer_free(Lexer *lexer);

/**
 * Check if the lexer has reached the end of the source
 *
 * @param lexer The lexer to check
 * @return true if at the end, false otherwise
 */
bool is_at_end(Lexer *lexer);

/**
 * Peek at the current character without advancing
 *
 * @param lexer The lexer to peek from
 * @return The current character
 */
char peek(Lexer *lexer);

/**
 * Peek at the next character without advancing
 *
 * @param lexer The lexer to peek from
 * @return The next character
 */
char peek_next(Lexer *lexer);

/**
 * Advance to the next character
 *
 * @param lexer The lexer to advnc_lex
 * @return The current character before advancing
 */
char advnc_lex(Lexer *lexer);

/**
 * Add a token to the token list
 *
 * @param lexer The lexer to add the token to
 * @param type The type of token
 * @param value The string value of the token
 */
void add_token(Lexer *lexer, TokenType type, const char *value);

/**
 * Skip whitespace characters
 *
 * @param lexer The lexer to skip whitespace in
 */
void skip_whitespace(Lexer *lexer);

/**
 * Process an identifier or keyword
 *
 * @param lexer The lexer to process from
 */
void identifier(Lexer *lexer);

/**
 * Process a number
 *
 * @param lexer The lexer to process from
 */
void number(Lexer *lexer);

/**
 * Scan a single token
 *
 * @param lexer The lexer to scan from
 */
void scan_token(Lexer *lexer);

/**
 * Tokenize the entire source
 *
 * @param lexer The lexer to tokenize
 * @return An array of tokens
 */
Token* tokenize(Lexer *lexer);

/**
 * Print a token (for debugging)
 *
 * @param token The token to print
 */
void print_token(const Token *token);

/**
 * Load file contents into memory
 *
 * @param path The path to the file
 * @param length A pointer to store the length of the file
 * @return The contents of the file
 */
char* read_file(const char* path, size_t *length);

/**
 * Get a string representation of a token type
 *
 * @param type The token type
 * @return A string representation of the token type
 */
const char* token_type_strings(TokenType type);

#endif /* LEXER_H */