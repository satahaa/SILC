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

Token lexer_next_token() {
    skip_whitespace();

    if (current_char == EOF) {
        return create_token(TOKEN_EOF, nullptr);
    }

    // Check for return keyword
    if (current_char == 'r') {
        char buffer[10];
        int i = 0;

        while (current_char != EOF && isalpha(current_char) && i < 9) {
            buffer[i++] = current_char;
            advance();
        }
        buffer[i] = '\0';

        if (strcmp(buffer, "ret") == 0) {
            return create_token(TOKEN_RETURN, allocate_string(buffer));
        } else {
            return create_token(TOKEN_UNKNOWN, allocate_string(buffer));
        }
    }
    if (current_char == 'l') {
        char buffer[10];
        int i = 0;

        while (current_char != EOF && isalpha(current_char) && i < 9) {
            buffer[i++] = current_char;
            advance();
        }
        buffer[i] = '\0';

        if (strcmp(buffer, "let") == 0) {
            return create_token(TOKEN_LET, allocate_string(buffer));
        } else {
            return create_token(TOKEN_UNKNOWN, allocate_string(buffer));
        }
    }
    if (isalpha(current_char)) {
        char buffer[32];
        int i = 0;

        while (current_char != EOF && isalnum(current_char) && i < 31) {
            buffer[i++] = current_char;
            advance();
        }
        buffer[i] = '\0';

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
        default: return "UNDEFINED";
    }
}

void token_free(Token* token) {
    if (token->value != NULL) {
        free(token->value);
        token->value = nullptr;
    }
}