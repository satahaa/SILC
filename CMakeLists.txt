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
        src/codegen.c
)

# Add executable for the project
add_executable(Cor ${SOURCES})

# Enable testing (Optional, if you want to use CTest)
#enable_testing()

# Add testing source files (if you want to include tests directly in the build)
#add_executable(test_lexer tests/test_lexer.c src/lexer.c)
#add_executable(test_parser tests/test_parser.c src/parser.c)
#add_executable(test_codegen tests/test_codegen.c src/codegen.c)

# Tests
#enable_testing()

# Compile test files
#add_executable(lexer_test tests/lexer_test.c src/lexer.c)
#add_executable(parser_test tests/parser_test.c src/parser.c src/lexer.c)
#add_executable(codegen_test tests/codegen_test.c src/codegen.c)
#add_executable(all_tests tests/main_test.c tests/lexer_test.c tests/parser_test.c tests/codegen_test.c
#               src/lexer.c src/parser.c src/codegen.c)

#target_include_directories(lexer_test PRIVATE include)
#target_include_directories(parser_test PRIVATE include)
#target_include_directories(codegen_test PRIVATE include)
#target_include_directories(all_tests PRIVATE include)

## Add tests
#add_test(NAME lexer_test COMMAND lexer_test)
#add_test(NAME parser_test COMMAND parser_test)
#add_test(NAME codegen_test COMMAND codegen_test)
#add_test(NAME all_tests COMMAND all_tests)
