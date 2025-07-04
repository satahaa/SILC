# Cor Compiler

Cor is a simple, dynamically‑typed programming language that compiles to C. It’s designed to be easy to learn and use, with a familiar C‑like syntax. This project, written in C, walks you through the fundamentals of compiler design—from lexical analysis all the way to code generation.

---

## Features

* **Dynamic Typing**: All numbers are `double`, text is `string`. Types are inferred at first assignment.
* **Variable Assignments**:

   * `let` keyword for declaration
   * `=` for re‑assignment
* **Return Statements**: `ret` exits the program with a status code.
* **Control Flow**:

   * **Conditionals**: `if` and `els`
   * **Loops**: `while`
   * **Loop Control**: `brk` (break) and `con` (continue). Restricted to one of each per loop block.
* **Operators**:

   * Arithmetic: `+`, `-`, `*`, `/`, `%`
   * Comparison: `==`, `!=`, `<`, `>`, `<=`, `>=`
   * Logical: `and`, `or`, `not`
   * Bitwise: `&`, `|`, `^`, `~`, `<<`, `>>` (operands cast to integers)
* **Input/Output**:

   * `out`: prints expressions or strings
   * `in`: reads a number or string into a variable
* **Parentheses Handling**: Override operator precedence with `(` and `)`
* **Error Handling**: Basic checks for syntax errors and invalid expressions

---

## Turing Completeness

Cor is Turing complete because it supports:

1. **Conditional Branching** (`if`/`els`)
2. **Indefinite Looping** (`while`)
3. **Memory Access** (mutable variables)

> Note: Practical limits exist (e.g., fixed buffer sizes), but Cor’s core is computationally universal.

---

# Installation

**Requirements**:

* A C compiler (GCC, Clang, etc.)
* CMake

**Build Steps**:

## Clone the repo
```bash
git clone https://github.com/satahaa/Cor.git
cd Cor
```
## Create and enter build dir
```bash
mkdir build && cd build
```
## Generate and build
```bash
cmake ..
cmake --build .
```
## Run the compiler
```bash
# Usage: ./Cor path/to/your/file.cor
```

---

# Usage

1. Write your Cor code in a `.cor` file.
2. Compile it using the generated `Cor` binary.
3. Enjoy!

**Example**: sieve of Eratosthenes in Cor

```cor
let limit = 100;
let n = 2;

out "Prime numbers up to 100:\n";

while n <= limit
{
    let is_prime = 1; 
    let i = 2;

    while i * i <= n
    {
        if n % i == 0
        {
            is_prime = 0;
            brk;
        }
        i = i + 1;
    }

    if is_prime == 1
    {
        out n;
    }

    n = n + 1;
}

ret 0;
```
The compiler parses the code, generates C output, and compiles it using GCC.

---

## Implementation

1. **Lexical Analysis**

   * Tokenizes keywords (`let`, `ret`, `if`, `els`, `while`, `brk`, `con`), identifiers, operators, numbers, strings, and delimiters.
2. **Parsing**

   * Uses a recursive descent parser to build a linear array of statements.
   * Enforces syntactical rules (e.g., `brk` and `con` only valid inside loops).
   * Stores expressions as token sequences for simplicity.
3. **Code Generation**

   * Translates the linear array of statements into equivalent C code.
   * Emits `strcpy` calls for string assignments.
   * Generates proper `if`/`else` blocks and `while` loops in C.
4. **Compilation Pipeline**

   * Reads the source file, tokenizes input, parses statements, generates C code, and invokes GCC to produce an executable.

---

## Future Improvements

* **String Concatenation**: Overload `+` for strings (generate `strcat`).
* **Alternative Block Syntax**: Support Python-style `:`/`end` blocks.
* **Functions & Scopes**: Add user-defined functions and proper variable scoping.
* **Arrays**: Implement native array data structures.
* **String indexing**: Allow accessing characters in strings (e.g., `str[0]`).
* **Enhanced Operators**: Add `+=`, `-=`, `*=` etc.
* **Improved Error Handling**: More descriptive messages and debug info.
* **Comments**: Support single-line (`//`) and multi-line (`/* ... */`) comments.
* **Standard Library**: Add common functions (e.g., `math`, `string` utilities).
* **Documentation**: Comprehensive guides and examples.

---

## Contact

Questions or suggestions? Open an issue or email me at [satahaa12345@gmail.com](mailto:satahaa12345@gmail.com).
