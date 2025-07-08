# Cor Language Design

This document outlines the design and implementation architecture of the Cor compiler.

## 1. Language Grammar

The current grammar of the Cor language is defined using a syntax similar to Extended Backus-Naur Form (EBNF):

```
Program         → Statement*
Statement       → LetStatement | ReturnStatement | IfStatement | WhileStatement | 
                  ExpressionStatement | OutStatement | InStatement | BreakStatement | ContinueStatement
LetStatement    → "let" identifier "=" Expression ";"
ReturnStatement → "ret" [Expression] ";"
IfStatement     → "if" "(" Expression ")" Block [ "else" Block ]
WhileStatement  → "while" "(" Expression ")" Block
ExpressionStatement → Expression ";"
OutStatement    → "out" Expression ";"
InStatement     → "in" identifier ";"
BreakStatement  → "brk" ";"
ContinueStatement → "con" ";"
Block           → "{" Statement* "}"
Expression      → Term ( ( "+" | "-" | "*" | "/" | "&&" | "||" | "==" | "!=" | "<" | ">" | "<=" | ">=" ) Term )*
Term            → identifier | number | string | "(" Expression ")" | UnaryOp Term
UnaryOp         → "!" | "-"
```

## 2. Core Language Features

-   **Dynamic Typing**: The language supports multiple types including `double` (default), `string`, and proper type inference.
-   **Variable Declaration**: Variables are declared and initialized using the `let` keyword (e.g., `let x = 10;`).
-   **Control Flow**:
    -   **Conditionals**: `if-else` statements for branching logic.
    -   **Loops**: `while` loops with proper `brk` (break) and `con` (continue) support.
    -   **Program Termination**: The `ret` statement exits the program with a specified status code.
-   **Input/Output**: 
    -   **Output**: `out` statement for displaying values.
    -   **Input**: `in` statement for reading user input into variables.
-   **Expressions**:
    -   **Arithmetic Operators**: `+`, `-`, `*`, `/`.
    -   **Logical Operators**: `&&` (AND), `||` (OR), `!` (NOT).
    -   **Comparison Operators**: `==`, `!=`, `<`, `>`, `<=`, `>=`.
    -   **Precedence**: Parentheses `()` can be used to override the default operator precedence.

## 3. Compiler Architecture

The compiler follows a traditional multi-stage pipeline: Lexical Analysis → Parsing → Semantic Analysis → Code Generation.

### 3.1. Lexical Analysis (`src/lexer.c`)

The lexer is a hand-written scanner that processes the raw source code into a stream of tokens.

-   **Responsibilities**:
    -   Recognizes keywords (`let`, `ret`, `if`, `else`, `while`, `out`, `in`, `brk`, `con`), identifiers, operators, integer literals, string literals, and delimiters.
    -   Strips whitespace and comments.
    -   Attaches position information (line, column) to each token for error reporting.

### 3.2. Parsing (`src/parser.c`)

The parser uses a recursive descent strategy to analyze the token stream and build an intermediate representation of the program.

-   **Intermediate Representation (IR)**: Instead of a traditional Abstract Syntax Tree (AST), the parser generates a simple **linear array of statement objects**. This simplifies the initial implementation, though it makes complex optimizations more challenging.
-   **Expression Handling**: Expressions are stored as sequences of tokens within each statement object, preserving their original structure for the code generator.
-   **Key Features**:
    -   Implemented `if-else` statements and `while` loops.
    -   Support for logical operators (`&&`, `||`, `!`) and comparison operators.
    -   Input/output statement parsing (`out`, `in`).
    -   Loop control statements (`brk`, `con`).
    -   Switched from `atoi()` to the safer `strtol()` for robust number parsing.
    -   Resolved memory management issues and added null checks to improve stability.

### 3.3. Semantic Analysis (`src/semantic.c`)

The semantic analyzer performs comprehensive validation of the parsed program before code generation.

-   **Symbol Table Management**:
    -   Implements a **scope stack** for proper variable scoping.
    -   Tracks variable declarations and usage across nested scopes.
    -   Supports block scoping for `if-else` and `while` statements.
    -   Validates variable visibility rules.

-   **Type System**:
    -   **Type Inference**: Automatically determines variable types from expressions.
    -   **Supported Types**: `TYPE_DOUBLE` (default), `TYPE_STRING`.
    -   **Type Checking**: Validates type compatibility in expressions and assignments.

-   **Validation Features**:
    -   **Variable Declaration Checking**: Prevents redeclaration of variables in the same scope.
    -   **Variable Usage Validation**: Ensures variables are declared before use.
    -   **Control Flow Validation**: Validates `brk` and `con` statements are only used within loops.
    -   **Loop Context Tracking**: Maintains loop depth to prevent misuse of loop control statements.

-   **Error Reporting**:
    -   `SEMANTIC_ERROR_UNDECLARED_VAR`: Variable used before declaration.
    -   `SEMANTIC_ERROR_REDECLARED_VAR`: Variable redeclared in same scope.
    -   `SEMANTIC_ERROR_TYPE_MISMATCH`: Type incompatibility errors.
    -   `SEMANTIC_ERROR_BREAK_OUTSIDE_LOOP`: `brk` statement outside loop.
    -   `SEMANTIC_ERROR_CONTINUE_OUTSIDE_LOOP`: `con` statement outside loop.

### 3.4. Code Generation (`src/codegen.c`)

The code generator traverses the linear statement array and transpiles it into C code.

-   **Process**:
    -   Translates Cor statements directly into their C equivalents.
    -   Maps Cor's dynamic typing to appropriate C types.
    -   Wraps all generated expressions in parentheses to ensure that Cor's operator precedence is correctly preserved in the final C code.
    -   Constructs valid C `if-else` blocks and `while` loops from the parsed statements.
    -   Generates proper C code for input/output operations.

### 3.5. Compilation Pipeline (`src/main.c`)

The `main` executable orchestrates the entire compilation process.

1.  **Input**: Reads a `.cor` source file specified via command-line arguments.
2.  **Pipeline Execution**: Initializes and runs the lexer, parser, semantic analyzer, and code generator in sequence.
3.  **Semantic Validation**: Performs comprehensive semantic analysis and aborts compilation if errors are found.
4.  **C Compilation**: Writes the generated C code to a temporary file.
5.  **Final Assembly**: Invokes the system's C compiler (GCC) to compile the C code into a native executable.
6.  **Cleanup**: Removes the intermediate C file and cleans up all compiler components.

## 4. Testing Strategy

The project uses a set of `.cor` files in the `tests/` directory to validate compiler correctness. The testing process focuses on verifying:

-   Correct evaluation of arithmetic, logical, and comparison expressions.
-   Proper handling of variable assignments and return values.
-   Accurate translation of `if-else` control flow structures.
-   Proper `while` loop execution with `brk` and `con` statements.
-   Input/output functionality with `out` and `in` statements.
-   Semantic validation of variable scoping and type checking.

## 5. Future Work

With the core language now Turing complete, supporting dynamic types, and comprehensive semantic analysis, future development can focus on adding more advanced features, improving performance, and enhancing the developer experience.

-   **User-Defined Functions**: The highest priority is to implement functions, including support for parameters, return values, and proper variable scoping (local vs. global). This is a critical step toward writing modular and reusable code.

-   **Arrays**: Introduce array or list data structures to handle collections of data. This would require significant updates to the parser and code generator to manage memory and access elements.

-   **AST-Based Intermediate Representation**: Replace the current linear statement array with a proper Abstract Syntax Tree (AST). An AST would provide a more structured representation of the code, enabling more complex analysis and future optimizations like constant folding or dead code elimination.

-   **Enhanced Type System**: Expand the type system to include integers, booleans, and custom user-defined types. Implement stricter type checking and type conversion rules.

-   **Standard Library**: Develop a small standard library with common utilities. This could include more advanced mathematical functions, file I/O operations, and a richer set of string manipulation functions.

-   **Enhanced Error Handling**: Improve the error reporting system to provide more specific and helpful messages, including precise line and column numbers and suggestions for fixes.

-   **Optimization**: Implement basic compiler optimizations such as constant folding, dead code elimination, and common subexpression elimination.
