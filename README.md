
# Cor Compiler

Cor is a simple and minimalistic compiler designed to handle basic tasks such as mathematical operations, control flow (for loops, if statements), functions, and return statements. The project is built using C and is aimed at helping understand the basics of compiler design and construction.

---

## Features

- **Integer Literals**: Supports integer literals and basic arithmetic operations.
- **Control Flow**: Implements simple `for` loops and `if` statements for basic decision-making and iteration.
- **Functions**: Supports the definition of functions with basic return types.
- **Return Statements**: Implements function return values.
- **Minimalistic Design**: Designed to be lightweight and simple, focusing on core compiler functionality.

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

To start using Cor, write a simple program in the language and run it through the compiler. Example:

```javascript
    let x = 10;
    let y = 20;
    if x < y
    {
        ret x;
    }
    ret y;
```

The compiler will parse and execute the program, returning the correct value based on the logic defined.

---

## Contributing

Contributions are welcome! If you want to contribute to Cor, feel free to fork the repository, create a branch, and submit a pull request with your changes.

### Steps to Contribute:

1. Fork the repository.
2. Create a new branch (`git checkout -b feature-name`).
3. Commit your changes (`git commit -m 'Add feature'`).
4. Push to the branch (`git push origin feature-name`).
5. Create a pull request.

---

## Acknowledgments

- Inspired by the need to understand basic compiler concepts.
- Thanks to all the open-source contributors and resources that helped in the development of this project.

---

## Contact

If you have any questions or suggestions, feel free to open an issue or contact me directly at `satahaa12345@gmail.com`.
