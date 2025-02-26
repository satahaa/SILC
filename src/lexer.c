#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <stdbool.h>
// Token types with descriptive names

typedef enum {
    TOKEN_LET,
    TOKEN_RET,
    TOKEN_INT,
    TOKEN_IDENTIFIER,
    TOKEN_ASSIGN,
    TOKEN_SEMICOLON,
    TOKEN_EOF
} TokenType;

// String representation of token types (for debugging)
const char* token_type_strings[] = {
    "LET", "RET", "INT", "IDENTIFIER", "ASSIGN", "SEMICOLON", "EOF"
};

typedef struct {
    TokenType type;
    char *value;
    size_t line;
    size_t column;
} Token;

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

// Keyword mapping structure
typedef struct {
    const char *keyword;
    TokenType type;
} Keyword;

// Define keywords
static const Keyword keywords[] = {
    {"let", TOKEN_LET},
    {"ret", TOKEN_RET},
    {NULL, 0} // Sentinel value
};

// Initialize lexer
Lexer* lexer_init(const char *source, const size_t length) {
    Lexer *lexer = malloc(sizeof(Lexer));
    if (!lexer) {
        fprintf(stderr, "Memory allocation failed for lexer\n");
        exit(EXIT_FAILURE);
    }

    lexer->source = malloc(length + 1);
    if (!lexer->source) {
        fprintf(stderr, "Memory allocation failed for source\n");
        free(lexer);
        exit(EXIT_FAILURE);
    }

    memcpy(lexer->source, source, length);
    lexer->source[length] = '\0';
    lexer->source_length = length;
    lexer->position = 0;
    lexer->line = 1;
    lexer->column = 1;

    // Start with a reasonable token capacity
    lexer->tokens_capacity = 32;
    lexer->tokens = malloc(sizeof(Token) * lexer->tokens_capacity);
    if (!lexer->tokens) {
        fprintf(stderr, "Memory allocation failed for tokens\n");
        free(lexer->source);
        free(lexer);
        exit(EXIT_FAILURE);
    }

    lexer->tokens_count = 0;

    return lexer;
}

// Free lexer resources
void lexer_free(Lexer *lexer) {
    if (!lexer) return;

    // Free all token values
    for (size_t i = 0; i < lexer->tokens_count; i++) {
        if (lexer->tokens[i].value != NULL)
            free(lexer->tokens[i].value);
    }

    free(lexer->tokens);
    free(lexer->source);
    free(lexer);
}

// Check if we've reached the end of the source
bool is_at_end(const Lexer *lexer) {
    return lexer->position >= lexer->source_length;
}

// Get current character without advancing
char peek(const Lexer *lexer) {
    if (is_at_end(lexer)) return '\0';
    return lexer->source[lexer->position];
}

// Get next character without advancing
char peek_next(const Lexer *lexer) {
    if (lexer->position + 1 >= lexer->source_length) return '\0';
    return lexer->source[lexer->position + 1];
}

// Advance to next character
char advnc_lex(Lexer *lexer) {
    const char c = lexer->source[lexer->position++];

    if (c == '\n') {
        lexer->line++;
        lexer->column = 1;
    } else {
        lexer->column++;
    }

    return c;
}

// Add a token to the token list
void add_token(Lexer *lexer, const TokenType type, const char *value) {
    // Ensure there's enough space
    if (lexer->tokens_count >= lexer->tokens_capacity) {
        lexer->tokens_capacity *= 2;
        lexer->tokens = realloc(lexer->tokens, sizeof(Token) * lexer->tokens_capacity);
        if (!lexer->tokens) {
            fprintf(stderr, "Memory reallocation failed for tokens\n");
            exit(EXIT_FAILURE);
        }
    }

    Token *token = &lexer->tokens[lexer->tokens_count++];
    token->type = type;
    token->value = strdup(value);
    token->line = lexer->line;
    token->column = lexer->column - strlen(value);
}

// Skip whitespace
void skip_whitespace(Lexer *lexer) {
    while (!is_at_end(lexer)) {
        const char c = peek(lexer);
        if (c == ' ' || c == '\t' || c == '\r' || c == '\n') {
            advnc_lex(lexer);
        } else {
            break;
        }
    }
}

// Process an identifier or keyword
void identifier(Lexer *lexer) {
    const size_t start_pos = lexer->position - 1;

    while (!is_at_end(lexer) && (isalnum(peek(lexer)) || peek(lexer) == '_')) {
        advnc_lex(lexer);
    }

    const size_t length = lexer->position - start_pos;
    char *text = malloc(length + 1);
    if (!text) {
        fprintf(stderr, "Memory allocation failed for identifier\n");
        exit(EXIT_FAILURE);
    }

    strncpy(text, &lexer->source[start_pos], length);
    text[length] = '\0';

    // Check if it's a keyword
    TokenType type = TOKEN_IDENTIFIER;
    for (int i = 0; keywords[i].keyword != NULL; i++) {
        if (strcmp(text, keywords[i].keyword) == 0) {
            type = keywords[i].type;
            break;
        }
    }

    add_token(lexer, type, text);
    free(text);
}

// Process a number
void number(Lexer *lexer) {
    const size_t start_pos = lexer->position - 1;

    while (!is_at_end(lexer) && isdigit(peek(lexer))) {
        advnc_lex(lexer);
    }

    const size_t length = lexer->position - start_pos;
    char *text = malloc(length + 1);
    if (!text) {
        fprintf(stderr, "Memory allocation failed for number\n");
        exit(EXIT_FAILURE);
    }

    strncpy(text, &lexer->source[start_pos], length);
    text[length] = '\0';

    add_token(lexer, TOKEN_INT, text);
    free(text);
}

// Scan a single token
void scan_token(Lexer *lexer) {
    const char c = advnc_lex(lexer);

    switch (c) {
        case ';': add_token(lexer, TOKEN_SEMICOLON, ";"); break;
        case '=': add_token(lexer, TOKEN_ASSIGN, "="); break;

        case ' ':
        case '\r':
        case '\t':
        case '\n':
            // Whitespace already handled by skip_whitespace
            break;

        default:
            if (isdigit(c)) {
                number(lexer);
            } else if (isalpha(c) || c == '_') {
                identifier(lexer);
            } else {
                fprintf(stderr, "Unexpected character '%c' at line %zu, column %zu\n",
                        c, lexer->line, lexer->column - 1);
            }
            break;
    }
}

// Tokenize the entire source
Token* tokenize(Lexer *lexer) {
    while (!is_at_end(lexer)) {
        skip_whitespace(lexer);
        if (!is_at_end(lexer)) {
            scan_token(lexer);
        }
    }

    // Add EOF token
    add_token(lexer, TOKEN_EOF, "EOF");

    return lexer->tokens;
}

// Print a token (for debugging)
void print_token(const Token *token) {
    printf("TOKEN: '%s', TYPE: %s, LINE: %zu, COLUMN: %zu\n",
           token->value, token_type_strings[token->type], token->line, token->column);
}

// Load file contents into memory
char* read_file(const char* path, size_t *length) {
    FILE *file = fopen(path, "rb");
    if (!file) {
        fprintf(stderr, "Could not open file '%s'\n", path);
        exit(EXIT_FAILURE);
    }

    fseek(file, 0, SEEK_END);
    *length = ftell(file);
    rewind(file);

    char *buffer = malloc(*length + 1);
    if (!buffer) {
        fprintf(stderr, "Not enough memory to read '%s'\n", path);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    const size_t bytes_read = fread(buffer, sizeof(char), *length, file);
    if (bytes_read < *length) {
        fprintf(stderr, "Could not read file '%s'\n", path);
        free(buffer);
        fclose(file);
        exit(EXIT_FAILURE);
    }

    buffer[*length] = '\0';
    fclose(file);
    return buffer;
}