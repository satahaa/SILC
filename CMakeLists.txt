cmake_minimum_required(VERSION 3.30)

# Project Name
project(SILC C)

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
        src/semantic.c
)

# Add executable for the project
add_executable(SILC ${SOURCES})

# Copy executable to source folder after build
add_custom_command(TARGET SILC POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:SILC> ${CMAKE_SOURCE_DIR}/
        COMMENT "Copying executable to source directory"
)
