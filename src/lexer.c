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

Token lexer_next_token() {
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
        if (strcmp(buffer, "if") == 0) {
            return create_token(TOKEN_IF, allocate_string(buffer));
        }
        if (strcmp(buffer, "els") == 0) {
            return create_token(TOKEN_ELSE, allocate_string(buffer));
        }
        if (strcmp(buffer, "and") == 0) {
            return create_token(TOKEN_AND, allocate_string(buffer));
        }
        if (strcmp(buffer, "or") == 0) {
            return create_token(TOKEN_OR, allocate_string(buffer));
        }
        if (strcmp(buffer, "return") == 0 ||
            strcmp(buffer, "int") == 0    ||
            strcmp(buffer, "long") == 0   ||
            strcmp(buffer, "char") == 0   ||
            strcmp(buffer, "short") == 0  ||
            strcmp(buffer, "float") == 0  ||
            strcmp(buffer, "double") == 0 ||
            strcmp(buffer, "void") == 0   ||
            strcmp(buffer, "for") == 0    ||
            strcmp(buffer, "while") == 0){
            fprintf(stderr, "Syntax error: Cannot use reserved keyword at line %d, column %d\n",
                    current_line, current_column);
            exit(EXIT_FAILURE);
        }

        return create_token(TOKEN_IDENT, allocate_string(buffer));
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
    //Operators and delimiters
    if (current_char == '+' ||
        current_char == '-' ||
        current_char == '*' ||
        current_char == '/' ||
        current_char == '%' ||
        current_char == '(' ||
        current_char == ')' ||
        current_char == ';' ||
        current_char == '=' ||
        current_char == '<' ||
        current_char == '>' ||
        current_char == '!' ||
        current_char == '{' ||
        current_char == '}') {
        bool advanced = false;
        char buffer[3];
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
            case ';': type = TOKEN_SEMICOLON; break;
            case '{': type = TOKEN_LBRACE; break;
            case '}': type = TOKEN_RBRACE; break;
            case '=':
                advance();
                if (current_char == '=') {
                    buffer[1] = current_char;
                    buffer[2] = '\0';
                    type = TOKEN_EQEQ;
                    advance();
                } else {
                    type = TOKEN_EQ;
                    advanced = true;
                }
                break;
            case '<':
                advance();
                if (current_char == '=') {
                    buffer[1] = current_char;
                    buffer[2] = '\0';
                    type = TOKEN_LTE;
                    advance();
                } else {
                    type = TOKEN_LT;
                    advanced = true;
                }
                break;
            case '>':
                advance();
                if (current_char == '=') {
                    buffer[1] = current_char;
                    buffer[2] = '\0';
                    type = TOKEN_GTE;
                    advance();
                } else {
                    type = TOKEN_GT;
                    advanced = true;
                }
                break;
            case '!':
                advance();
                if (current_char == '=') {
                    buffer[1] = current_char;
                    buffer[2] = '\0';
                    type = TOKEN_NEQ;
                    advance();
                } else {
                    type = TOKEN_NOT;
                    advanced = true;
                }
                break;
            default: type = TOKEN_UNKNOWN;
        }

        if (!advanced) advance();
        return create_token(type, allocate_string(buffer));
    }

    // Unknown token
    char value[2];
    value[0] = current_char;
    value[1] = '\0';
    advance();
    return create_token(TOKEN_UNKNOWN, allocate_string(value));
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
        case TOKEN_EQEQ: return "EQEQ";
        case TOKEN_NEQ: return "NEQ";
        case TOKEN_LT: return "LT";
        case TOKEN_GT: return "GT";
        case TOKEN_LTE: return "LTE";
        case TOKEN_GTE: return "GTE";
        case TOKEN_AND: return "AND";
        case TOKEN_OR: return "OR";
        case TOKEN_NOT: return "NOT";
        case TOKEN_IF: return "IF";
        case TOKEN_ELSE: return "ELSE";
        case TOKEN_LBRACE: return "LBRACE";
        case TOKEN_RBRACE: return "RBRACE";
        default: return "UNDEFINED";
    }
}

void token_free(Token* token) {
    if (token->value != NULL) {
        free(token->value);
        token->value = nullptr;
    }
}

void lexer_cleanup() {
    // Nothing to clean up for now
}
