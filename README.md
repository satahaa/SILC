# Cor Compiler

Cor is a simple and minimalistic compiler designed to handle mathematical operations, variable assignments, return statements, and basic control flow. The project is built using C and focuses on understanding the fundamentals of compiler design and implementation.

---

## Features

- **Integer Literals**: Supports integer literals and basic arithmetic operations.
- **Variable Assignments**: Implements variable declaration using the `let` keyword.
- **Return Statements**: Supports returning values using the `ret` keyword.
- **Control Flow (Future Work)**: Planned support for `if` statements and loops.
- **Minimalistic Design**: Lightweight and simple, focusing on core compiler functionality.

---

## Installation

### Requirements

- **C Compiler** (e.g., GCC, Clang)
- **CMake** (for building the project)

### Steps

1. Clone the repository:
   ```bash
   git clone https://github.com/satahaa/Cor.git
   ```
2. Navigate to the project directory:
   ```bash
   cd Cor
   ```
3. Create a build directory:
   ```bash
   mkdir build
   cd build
   ```
4. Generate build files using CMake:
   ```bash
   cmake ..
   ```
5. Build the project:
   ```bash
   cmake --build .
   ```
6. Run the compiled executable:
   ```bash
   ./Cor
   ```

---

## Usage

To use Cor, write a simple program and run it through the compiler. Example:

```cor
let x = 10;
let y = 20;
ret x + y;
```

The compiler parses the code, generates C output, and compiles it using GCC.

---

## Implementation

### 1. Lexical Analysis
- Tokenizes keywords (`let`, `ret`), identifiers, operators, numbers, and delimiters.
- Uses safer `strtol()` for number parsing.

### 2. Parsing
- Implements a recursive descent parser.
- Builds an Abstract Syntax Tree (AST) for statements and expressions.

### 3. Code Generation
- Converts AST into equivalent C code.
- Wraps expressions in parentheses to preserve operator precedence.

### 4. Compilation Pipeline
- Reads the source file, tokenizes input, generates an AST, converts it to C, and invokes GCC.

---

## Contributing

Contributions are welcome! If you want to contribute to Cor, fork the repository, create a branch, and submit a pull request with your changes.

### Steps to Contribute:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-name`).
3. Commit your changes (`git commit -m 'Add feature'`).
4. Push to the branch (`git push origin feature-name`).
5. Create a pull request.

---

## Future Improvements

- **Support for conditionals (`if`/`else`)**
- **Implement functions and scopes**
- **Add more data types (strings, booleans)**
- **Improve error handling**

---

## Contact

If you have any questions or suggestions, feel free to open an issue or contact me at `satahaa12345@gmail.com`.

