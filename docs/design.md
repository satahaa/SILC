# Cor Language Design

This document outlines the design and implementation architecture of the Cor compiler.

## 1. Language Grammar

The current grammar of the Cor language is defined using a syntax similar to Extended Backus-Naur Form (EBNF):

```
Program         → Statement*
Statement       → LetStatement | ReturnStatement | IfStatement
LetStatement    → "let" identifier "=" Expression ";"
ReturnStatement → "ret" [Expression] ";"
IfStatement     → "if" "(" Expression ")" Block [ "else" Block ]
Block           → "{" Statement* "}"
Expression      → Term ( ( "+" | "-" | "*" | "/" | "&&" | "||" ) Term )*
Term            → identifier | number | "(" Expression ")" | UnaryOp Term
UnaryOp         → "!"
```

## 2. Core Language Features

-   **Static Typing**: The language currently supports a single `long long` integer type for all values.
-   **Variable Declaration**: Variables are declared and initialized using the `let` keyword (e.g., `let x = 10;`).
-   **Control Flow**:
    -   **Conditionals**: `if-else` statements for branching logic.
    -   **Program Termination**: The `ret` statement exits the program with a specified status code.
-   **Expressions**:
    -   **Arithmetic Operators**: `+`, `-`, `*`, `/`.
    -   **Logical Operators**: `&&` (AND), `||` (OR), `!` (NOT).
    -   **Precedence**: Parentheses `()` can be used to override the default operator precedence.

## 3. Compiler Architecture

The compiler follows a traditional multi-stage pipeline: Lexical Analysis → Parsing → Code Generation.

### 3.1. Lexical Analysis (`src/lexer.c`)

The lexer is a hand-written scanner that processes the raw source code into a stream of tokens.

-   **Responsibilities**:
    -   Recognizes keywords (`let`, `ret`, `if`, `else`), identifiers, operators, integer literals, and delimiters.
    -   Strips whitespace and comments.
    -   Attaches position information (line, column) to each token for error reporting.

### 3.2. Parsing (`src/parser.c`)

The parser uses a recursive descent strategy to analyze the token stream and build an intermediate representation of the program.

-   **Intermediate Representation (IR)**: Instead of a traditional Abstract Syntax Tree (AST), the parser generates a simple **linear array of statement objects**. This simplifies the initial implementation, though it makes complex optimizations more challenging.
-   **Expression Handling**: Expressions are stored as sequences of tokens within each statement object, preserving their original structure for the code generator.
-   **Key Improvements**:
    -   Implemented `if-else` statements and logical operators (`&&`, `||`, `!`).
    -   Switched from `atoi()` to the safer `strtol()` for robust number parsing.
    -   Resolved memory management issues and added null checks to improve stability.

### 3.3. Code Generation (`src/codegen.c`)

The code generator traverses the linear statement array and transpiles it into C code.

-   **Process**:
    -   Translates Cor statements directly into their C equivalents.
    -   Maps Cor's `long long` type for all variables.
    -   Wraps all generated expressions in parentheses to ensure that Cor's operator precedence is correctly preserved in the final C code.
    -   Constructs valid C `if-else` blocks from the parsed conditional statements.

### 3.4. Compilation Pipeline (`src/main.c`)

The `main` executable orchestrates the entire compilation process.

1.  **Input**: Reads a `.cor` source file specified via command-line arguments.
2.  **Pipeline Execution**: Initializes and runs the lexer, parser, and code generator in sequence.
3.  **C Compilation**: Writes the generated C code to a temporary file.
4.  **Final Assembly**: Invokes the system's C compiler (GCC) to compile the C code into a native executable.
5.  **Cleanup**: Removes the intermediate C file.

## 4. Testing Strategy

The project uses a set of `.cor` files in the `tests/` directory to validate compiler correctness. The testing process focuses on verifying:

-   Correct evaluation of arithmetic and logical expressions.
-   Proper handling of variable assignments and return values.
-   Accurate translation of `if-else` control flow structures.

## 5. Future Work

With the core language now Turing complete and supporting dynamic types, future development can focus on adding more advanced features, improving performance, and enhancing the developer experience.

-   **User-Defined Functions**: The highest priority is to implement functions, including support for parameters, return values, and proper variable scoping (local vs. global). This is a critical step toward writing modular and reusable code.

-   **Arrays**: Introduce array or list data structures to handle collections of data. This would require significant updates to the parser and code generator to manage memory and access elements.

-   **AST-Based Intermediate Representation**: Replace the current linear statement array with a proper Abstract Syntax Tree (AST). An AST would provide a more structured representation of the code, enabling more complex analysis and future optimizations like constant folding or dead code elimination.

-   **Standard Library**: Develop a small standard library with common utilities. This could include more advanced mathematical functions, file I/O operations, and a richer set of string manipulation functions.

-   **Enhanced Error Handling**: Improve the error reporting system to provide more specific and helpful messages, including precise line and column numbers and suggestions for fixes.