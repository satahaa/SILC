# Cor Language Design Documentation

## Current Grammar
The Cor language now supports the following grammar elements:

```
Program         → Statement*
Statement       → LetStatement | ReturnStatement | IfStatement
LetStatement    → "let" identifier "=" Expression ";"
ReturnStatement → "ret" [Expression] ";"
IfStatement     → "if" "(" Expression ")" Block [ "else" Block ]
Block           → "{" Statement* "}"
Expression      → Term ( ("+"|"-"|"*"|"/"|"&&"|"||") Term )*
Term            → identifier | number | "(" Expression ")" | UnaryOp Term
UnaryOp         → "!"
```

## Language Features
- **Variables**: Defined using the `let` keyword
- **Return statements**: Using the `ret` keyword
- **Expressions**: Support for basic arithmetic and logical operations (`&&`, `||`, `!`)
- **Conditionals**: `if` and `if-else` statements
- **Data types**: Currently supports integers

## Implementation Process

### 1. Lexical Analysis
The lexer (`src/lexer.c`) reads the source code and converts it into tokens:

- **Keywords**: `let`, `ret`, `if`, `else`
- **Identifiers**: Variable names
- **Operators**: `+`, `-`, `*`, `/`, `=`, `&&`, `||`, `!`
- **Numbers**: Integer literals
- **Delimiters**: `;`, `{`, `}`, `(`, `)`

Tokens contain type, value, and position information for error reporting.

### 2. Parsing
The parser (`src/parser.c`) implements a recursive descent approach:

- Stores the program as a **linear array of statements** rather than an AST
- Handles statements (`let`, `return`, `if-else`)
- Parses expressions using a **basic precedence system**
- Supports **logical operations (`&&`, `||`, `!`)** for conditional expressions
- Stores expressions as token sequences for simplicity

#### Recent improvements:
- Implemented **if-else statements**
- Added **logical operators (`&&`, `||`, `!`)** to expressions
- Changed from `atoi()` to **safer `strtol()`** for number parsing
- Fixed **memory management** in `expression_free()`
- Added **proper null checking** to prevent potential crashes

### 3. Code Generation
The code generator (`src/codegen.c`):

- Takes the **linear array of statements** from the parser
- Translates it to equivalent **C code**
- Uses `long long` types to handle **larger integer values**
- Wraps expressions in parentheses to preserve **operator precedence**
- Generates **if-else blocks** correctly in the output code

### 4. Compilation Pipeline
The main program (`src/main.c`):

- Parses **command line arguments** for input/output files
- Initializes the **lexer** with source code
- Runs the **parser** to build the statement array
- Generates **C code** from the statement array
- Invokes **GCC** to compile the generated C code
- Cleans up **intermediate files**

### 5. Testing
Using test cases like `tests/test.cor` to verify:

- **Expression evaluation accuracy**
- **Variable assignments**
- **Return value handling**
- **Conditional execution (`if-else`)**
- **Logical operators (`&&`, `||`, `!`)**

## Future Improvements
Potential enhancements:
- **Implement loops** (`while`, `for`)
- **Add support for functions and scope**
- **Support for additional data types** (strings, booleans)
- **Improve error handling and debugging support**

