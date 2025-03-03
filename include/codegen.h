#ifndef CODEGEN_H
#define CODEGEN_H

#include "parser.h"

// Initialize the code generator
void codegen_init(const char* output_file);

// Generate assembly code from the AST
void codegen_generate(Program program);

// Free resources used by the code generator
void codegen_cleanup();

#endif // CODEGEN_H