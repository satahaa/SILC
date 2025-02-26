#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
typedef enum {
    INT,
} TokenLiteral;

typedef enum {
    LET, RET
} TokenKeyword;

typedef enum {
    SEMI, OPEN, CLOSE,
} TokenDelimiter;

typedef struct {
    union {
        TokenLiteral ltype;
        TokenDelimiter dtype;
        TokenKeyword ktype;
    } type;
    int val;
} Token;

Token *num_gen(char c, FILE *file) {
    Token *token = malloc(sizeof(Token));
    token->type.ltype = INT;

    char word[20];
    int i = 0;

    word[i++] = c;
    c = (char)fgetc(file);
    while (isdigit(c) && i < 99) {
        word[i++] = c;
        c = (char)fgetc(file);
        if (!isdigit(c)) {
            ungetc(c, file);
            break;
        }
    }

    word[i] = '\0';
    token->val = atoi(word);

    return token;
}
Token *keyword_gen(char c, FILE *file) {
    Token *token = malloc(sizeof(Token));
    char word[10];
    int i = 0;

    word[i++] = c;
    c = (char)fgetc(file);
    while (isalpha(c) && i < 99) {
        word[i++] = c;
        c = (char)fgetc(file);
        if (!isalpha(c)) {
            ungetc(c, file);
            break;
        }
    }

    word[i] = '\0';
    if (!strcmp(word, "let")) {
        token->type.ktype = LET;
    } else if (!strcmp(word, "ret")) {
        token->type.ktype = RET;
    }

    return token;
}

void lex(FILE *file) {
    char c  = fgetc(file);
    while (c != EOF) {
        while (c == ' ' || c == '\n')
            c = (char)fgetc(file);

        if (isdigit(c)) { //number
            Token *t = num_gen(c, file);
            printf("LIT: %d\n", t->val);
            free(t);
        } else if (isalpha(c)){ //keyword
            Token *t = keyword_gen(c, file);
            if (t->type.ktype == LET) {
                c = (char)fgetc(file);
                printf("LET\n %c\n", c);
            } else if (t->type.ktype == RET) {
                printf("RETURN\n");
            }
            free(t);
        }
        else printf("%c\n", c); //semi

        c = (char)fgetc(file);
    }
}
int main() {
    FILE *file;
    file = fopen("../tests/test.cor", "r");
    lex(file);
    fclose(file);
}

