cmake_minimum_required(VERSION 3.30)

# Project Name
project(Cor C)

# Set the C standard
set(CMAKE_C_STANDARD 23)

# Add include directory for header files
include_directories(include)

# Add all the source files
set(SOURCES
        src/main.c
        src/lexer.c
        src/parser.c
        src/semantic.c
        src/codegen.c
        src/utils.c
)

# Add executable for the project
add_executable(Cor ${SOURCES})

# Enable testing (Optional, if you want to use CTest)
enable_testing()

# Add testing source files (if you want to include tests directly in the build)
add_executable(test_lexer tests/test_lexer.c src/lexer.c)
add_executable(test_parser tests/test_parser.c src/parser.c)
add_executable(test_codegen tests/test_codegen.c src/codegen.c)

# Link the test executables with the Cor executable if needed
# target_link_libraries(test_lexer Cor)
# target_link_libraries(test_parser Cor)
# target_link_libraries(test_codegen Cor)

# Optionally, add testing functionality
# add_test(NAME LexerTest COMMAND test_lexer)
# add_test(NAME ParserTest COMMAND test_parser)
# add_test(NAME CodegenTest COMMAND test_codegen)
