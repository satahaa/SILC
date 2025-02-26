#include <stdio.h>
#include <stdlib.h>
#include "lexer.h"
#include "parser.h"
int main(const int argc, char* argv[]) {
    // Check if file path is provided
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <path>\n", argv[0]);
        return EXIT_FAILURE;
    }

    // Read the source file
    size_t length;
    char* source = read_file(argv[1], &length);
    if (!source) {
        return EXIT_FAILURE;
    }

    // Print the source code
    printf("Source code:\n");
    printf("--------------------------------------------------\n");
    printf("%s\n", source);
    printf("--------------------------------------------------\n\n");

    // Initialize the lexer and tokenize
    Lexer* lexer = lexer_init(source, length);
    tokenize(lexer);

    // Print all tokens
    printf("Tokens:\n");
    printf("--------------------------------------------------\n");
    for (size_t i = 0; i < lexer->tokens_count; i++) {
        const Token* token = &lexer->tokens[i];
        printf("%-12s | %-15s | Line %-3zu | Col %-3zu\n",
               token->value,
               token_type_to_string(token->type),
               token->line,
               token->column);
    }
    printf("--------------------------------------------------\n\n");

    // Initialize the parser and parse the program
    Parser* parser = parser_init(lexer);
    AstNode* ast = parse_program(parser);

    // Check if parsing was successful
    if (parser->hadError) {
        printf("Parsing failed with errors.\n");
    } else {
        printf("Parsing successful! AST:\n");
        printf("--------------------------------------------------\n");
        print_ast(ast, 0);
        printf("--------------------------------------------------\n");
    }

    // Clean up
    free_ast(ast);
    parser_free(parser);
    lexer_free(lexer);
    free(source);

    return parser->hadError ? EXIT_FAILURE : EXIT_SUCCESS;
}