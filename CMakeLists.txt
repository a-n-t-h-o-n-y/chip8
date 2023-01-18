cmake_minimum_required(VERSION 3.24)
project(chip8)

# Dependencies
add_subdirectory(${PROJECT_SOURCE_DIR}/external/escape/)

# Add the source files for the interpreter
add_executable(chip8
    src/main.cpp
)

# Set the C++ standard to C++20 for the chip8 target
target_compile_features(chip8 PRIVATE cxx_std_20)

# Link the interpreter with any necessary libraries
target_link_libraries(chip8 PRIVATE
    escape
)

add_executable(test_chip8
    test/test.cpp
)

target_compile_features(test_chip8 PRIVATE cxx_std_20)
target_link_libraries(test_chip8 PRIVATE
    escape
)
