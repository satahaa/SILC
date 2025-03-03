#include <stdio.h>
#include <stdlib.h>
#include <process.h>
#include <string.h>
#include <direct.h>
#include <windows.h>
#include "lexer.h"
#include "parser.h"
#include "codegen.h"

// Function to check if a program is installed
int is_program_installed(const char* program_name) {
    char cmd[100];
    sprintf(cmd, "where %s >nul 2>nul", program_name);
    return system(cmd) == 0;
}

// Function to download and install NASM
int install_nasm() {
    printf("NASM not found. Attempting to download and install...\n");

    // Create a temporary directory
    _mkdir("temp");

    // Download NASM (using PowerShell)
    const char* download_cmd =
        "powershell -Command \"[Net.ServicePointManager]::SecurityProtocol = "
        "[Net.SecurityProtocolType]::Tls12; Invoke-WebRequest "
        "-Uri 'https://www.nasm.us/pub/nasm/releasebuilds/2.15.05/win64/nasm-2.15.05-win64.zip' "
        "-OutFile 'temp\\nasm.zip'\"";

    printf("Downloading NASM...\n");
    if (system(download_cmd) != 0) {
        fprintf(stderr, "Failed to download NASM\n");
        return 0;
    }

    // Extract the ZIP file (using PowerShell)
    const char* extract_cmd =
        "powershell -Command \"Expand-Archive -Path 'temp\\nasm.zip' -DestinationPath 'temp' -Force\"";

    printf("Extracting NASM...\n");
    if (system(extract_cmd) != 0) {
        fprintf(stderr, "Failed to extract NASM\n");
        return 0;
    }

    // Add NASM to the PATH for the current process
    char current_dir[MAX_PATH];
    _getcwd(current_dir, MAX_PATH);

    char nasm_path[MAX_PATH * 2];
    sprintf(nasm_path, "%s\\temp\\nasm-2.15.05", current_dir);

    // Get the current PATH
    char old_path[32767]; // Maximum environment variable size
    GetEnvironmentVariable("PATH", old_path, sizeof(old_path));

    // Append NASM directory to PATH
    char new_path[32767 + MAX_PATH * 2];
    sprintf(new_path, "%s;%s", old_path, nasm_path);
    SetEnvironmentVariable("PATH", new_path);

    printf("NASM installed temporarily for this session\n");
    return 1;
}

// Function to assemble and link
int assemble_and_link(const char* asm_file, const char* obj_file, const char* exe_file) {
    // Check if NASM is installed
    if (!is_program_installed("nasm")) {
        // Try to install NASM
        if (!install_nasm()) {
            fprintf(stderr, "Error: NASM could not be installed automatically\n");
            return 0;
        }
    }

    // Assemble the code using NASM
    char nasm_cmd[256];
    sprintf(nasm_cmd, "nasm -f win64 %s -o %s", asm_file, obj_file);
    printf("Running: %s\n", nasm_cmd);

    int nasm_result = system(nasm_cmd);
    if (nasm_result != 0) {
        fprintf(stderr, "Error: Failed to assemble the code\n");
        return 0;
    }

    // Try to link using GCC (MinGW)
    if (is_program_installed("gcc")) {
        char gcc_cmd[256];
        sprintf(gcc_cmd, "gcc -o %s %s -lkernel32", exe_file, obj_file);
        printf("Running: %s\n", gcc_cmd);

        if (system(gcc_cmd) == 0) {
            return 1; // Success
        }
    }

    fprintf(stderr, "Error: Could not link the object file. Neither MSVC linker nor GCC found.\n");
    return 0;
}

int main() {
    const char* input_file = "../tests/test.cor";
    const char* asm_file = "output.asm";
    const char* obj_file = "output.obj";
    const char* exe_file = "output.exe";

    // Check if input file exists
    FILE* source = fopen(input_file, "r");
    if (source == NULL) {
        fprintf(stderr, "Error: Could not open input file %s\n", input_file);
        return 1;
    }

    // Initialize the compiler components
    lexer_init(source);
    parser_init();
    codegen_init(asm_file);

    // Parse the input
    Program program = parser_parse();

    // Generate code
    codegen_generate(program);

    // Clean up resources
    program_free(&program);
    parser_cleanup();
    lexer_cleanup();
    codegen_cleanup();

    fclose(source);

    printf("Assembly code generated. Now assembling and linking...\n");

    // Assemble and link
    if (!assemble_and_link(asm_file, obj_file, exe_file)) {
        return 1;
    }

    printf("Compilation completed successfully. Executable created: %s\n", exe_file);

    // Optional: Clean up intermediate files
    //remove(asm_file);
    //remove(obj_file);

    return 0;
}