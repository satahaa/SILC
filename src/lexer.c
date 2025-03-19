#include <stdlib.h>
#include <string.h>

#include <ctype.h>
#include "lexer.h"

static FILE* source;
static int current_line = 1;
static int current_column = 0;
static char current_char;

void lexer_init(FILE* source_file) {
    source = source_file;
    current_line = 1;
    current_column = 0;
    current_char = (char)fgetc(source);
}

static void advance() {
    if (current_char == '\n') {
        current_line++;
        current_column = 0;
    } else {
        current_column++;
    }
    current_char = (char)fgetc(source);
}

static void skip_whitespace() {
    while (current_char != EOF && isspace(current_char)) {
        advance();
    }
}

static char* allocate_string(const char* str) {

    const size_t len = strlen(str);
    const auto result = (char*)malloc(len + 1);
    if (result == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }

    strcpy(result, str);
    return result;
}

static Token create_token(const Ttype type, char* value) {
    Token token;
    token.type = type;
    token.value = value;
    token.line = current_line;
    token.column = current_column;
    return token;
}

Token lexer_next_token(const Token current_token) {
    skip_whitespace();

    if (current_char == EOF) {
        return create_token(TOKEN_EOF, nullptr);
    }

if (isalpha(current_char)) {
        char buffer[32];
        int i = 0;

        while (current_char != EOF && (isalnum(current_char) || current_char == '_') && i < 31) {
            buffer[i++] = current_char;
            advance();
        }
        buffer[i] = '\0';

        // Check for keywords
        if (strcmp(buffer, "let") == 0) {
            return create_token(TOKEN_LET, allocate_string(buffer));
        }
        if (strcmp(buffer, "ret") == 0) {
            return create_token(TOKEN_RETURN, allocate_string(buffer));
        }
        if (strcmp(buffer, "return") == 0 || strcmp(buffer, "int") == 0) {
            fprintf(stderr, "Syntax error: Cannot use reserved keyword at line %d, column %d\n",
                    current_line, current_column);
            exit(EXIT_FAILURE);
        }

        return create_token(TOKEN_IDENT, allocate_string(buffer));
}

    if (current_char == '=') {
        char buffer[2];
        buffer[0] = current_char;
        buffer[1] = '\0';
        advance();
        return create_token(TOKEN_EQ, allocate_string(buffer));
    }
    // Check for numbers
    if (isdigit(current_char)) {
        char buffer[32];
        int i = 0;

        while (current_char != EOF && isdigit(current_char) && i < 31) {
            buffer[i++] = current_char;
            advance();
        }
        buffer[i] = '\0';

        return create_token(TOKEN_NUMBER, allocate_string(buffer));
    }
    //Operators
    if (current_char == '+' ||
        current_char == '-' ||
        current_char == '*' ||
        current_char == '/' ||
        current_char == '%' ||
        current_char == '(' ||
        current_char == ')') {

        char buffer[2];
        buffer[0] = current_char;
        buffer[1] = '\0';

        Ttype type;
        switch (buffer[0]) {
            case '+': type = TOKEN_PLUS; break;
            case '-': type = TOKEN_MINUS; break;
            case '*': type = TOKEN_MUL; break;
            case '/': type = TOKEN_DIV; break;
            case '%': type = TOKEN_MOD; break;
            case '(': type = TOKEN_LPAREN; break;
            case ')': type = TOKEN_RPAREN; break;
            default: type = TOKEN_UNKNOWN;
        }

        advance();
        return create_token(type, allocate_string(buffer));
    }
    // Check for semicolon
    if (current_char == ';') {
        char* value = malloc(2);
        if (value == NULL) {
            fprintf(stderr, "Memory allocation error\n");
            exit(1);
        }
        value[0] = current_char;
        value[1] = '\0';
        advance();
        return create_token(TOKEN_SEMICOLON, value);
    }

    // Unknown token
    char* value = malloc(2);
    if (value == NULL) {
        fprintf(stderr, "Memory allocation error\n");
        exit(1);
    }
    value[0] = current_char;
    value[1] = '\0';
    advance();
    return create_token(TOKEN_UNKNOWN, value);
}

void lexer_cleanup() {
    // Nothing to clean up for now
}

const char* token_type_to_string(const Ttype type) {
    switch (type) {
        case TOKEN_RETURN: return "RETURN";
        case TOKEN_NUMBER: return "NUMBER";
        case TOKEN_SEMICOLON: return "SEMICOLON";
        case TOKEN_EOF: return "EOF";
        case TOKEN_UNKNOWN: return "UNKNOWN";
        case TOKEN_LET: return "LET";
        case TOKEN_IDENT: return "IDENT";
        case TOKEN_EQ: return "EQ";
        case TOKEN_PLUS: return "PLUS";
        case TOKEN_MINUS: return "MINUS";
        case TOKEN_MUL: return "MUL";
        case TOKEN_DIV: return "DIV";
        case TOKEN_MOD: return "MOD";
        case TOKEN_LPAREN: return "LPAREN";
        case TOKEN_RPAREN: return "RPAREN";
        default: return "UNDEFINED";
    }
}

void token_free(Token* token) {
    if (token->value != NULL) {
        free(token->value);
        token->value = nullptr;
    }
}