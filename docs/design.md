# Cor Language Design Documentation

## Current Grammar
The Cor language currently supports a minimal grammar with the following elements:

```
Program         → Statement*
Statement       → LetStatement | ReturnStatement
LetStatement    → "let" identifier "=" Expression ";"
ReturnStatement → "ret" [Expression] ";"
Expression      → Term (("+"|"-"|"*"|"/") Term)*
Term            → identifier | number | "(" Expression ")"
```

## Language Features
- **Variables**: Define using `let` keyword
- **Return statements**: Using `ret` keyword
- **Expressions**: Support for basic arithmetic operations
- **Data types**: Currently only supports integers

## Implementation Process

### 1. Lexical Analysis
The lexer (`src/lexer.c`) reads the source code and converts it into tokens:

- **Keywords**: `let`, `ret`
- **Identifiers**: Variable names
- **Operators**: `+`, `-`, `*`, `/`, `=`
- **Numbers**: Integer literals
- **Delimiters**: `;`, `(`, `)`

Tokens contain type, value, and position information for error reporting.

### 2. Parsing
The parser (`src/parser.c`) implements a recursive descent approach:

- Builds an **Abstract Syntax Tree (AST)** from tokens
- Handles statements (`let`, `return`)
- Parses expressions using a **basic precedence system**
- Stores expressions as token sequences for simplicity

#### Recent improvements:
- Changed from `atoi()` to **safer `strtol()`** for number parsing
- Fixed **memory management** in `expression_free()`
- Added **proper null checking** to prevent potential crashes

### 3. Code Generation
The code generator (`src/codegen.c`):

- Takes the **AST from the parser**
- Translates it to equivalent **C code**
- Uses `long long` types to handle **larger integer values**
- Wraps expressions in parentheses to preserve **operator precedence**

### 4. Compilation Pipeline
The main program (`src/main.c`):

- Parses **command line arguments** for input/output files
- Initializes the **lexer** with source code
- Runs the **parser** to build the AST
- Generates **C code** from the AST
- Invokes **GCC** to compile the generated C code
- Cleans up **intermediate files**

### 5. Testing
Using test cases like `tests/test.cor` to verify:

- **Expression evaluation accuracy**
- **Variable assignments**
- **Return value handling**

## Future Improvements
Potential enhancements:
- **Add support for conditionals** (`if/else`)
- **Implement functions and scope**
- **Support for additional data types** (strings, booleans)
- **Add more robust error handling**

